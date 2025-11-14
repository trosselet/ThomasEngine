#include <Render/pch.h>
#include <Render/Header/Mesh.h>
#include <Render/Header/Geometry.h>
#include <Render/Header/Render.h>
#include <Render/Header/UploadBuffer.h>
#include <Render/Header/RenderResources.h>

Mesh::Mesh(Geometry* pGeometry, Render* pRender, bool deferUpload)
{
	m_pGeometry = pGeometry;
	m_pRender = pRender;

    if (deferUpload)
    {
        UploadGeometry(true);
    }
    else
    {
        UploadGeometry(false);
    }

}

Mesh::~Mesh()
{
    if (m_pVertexBufferGPU)
    {
        m_pVertexBufferGPU->Release();
        m_pVertexBufferGPU = nullptr;
    }
    
    if (m_pIndexBufferGPU)
    {
        m_pIndexBufferGPU->Release();
        m_pIndexBufferGPU = nullptr;
    }

	ReleaseUploadBuffers();
}

D3D12_INDEX_BUFFER_VIEW Mesh::GetIndexBuffer()
{
	return m_indexBuffer;
}

D3D12_VERTEX_BUFFER_VIEW Mesh::GetVertexBuffer()
{
	return m_vertexBuffer;
}

UINT Mesh::GetIndexCount()
{
	return static_cast<UINT>(m_pGeometry->indexNumber);
}

void Mesh::ReleaseUploadBuffers()
{
    if (m_pIndexBufferUploader)
    {
        m_pIndexBufferUploader->Release();
        m_pIndexBufferUploader = nullptr;
    }

    if (m_pVertexBufferUploader)
    {
        m_pVertexBufferUploader->Release();
        m_pVertexBufferUploader = nullptr;
    }
}

void Mesh::UploadGeometry(bool deferred)
{
    if (!m_pGeometry) return;

    std::vector<float32> geometryData;
    int32 floatStride = 3;

    bool hasColor = !m_pGeometry->colors.empty();
    bool hasUv = !m_pGeometry->UVs.empty();
    bool hasNormal = !m_pGeometry->normals.empty();

    if (hasColor) floatStride += 4;
    if (hasUv)    floatStride += 2;
    if (hasNormal)floatStride += 3;

    size_t vertexCount = m_pGeometry->positions.size();
    geometryData.reserve(vertexCount * floatStride);

    for (size_t i = 0; i < vertexCount; ++i)
    {
        geometryData.push_back(m_pGeometry->positions[i].x);
        geometryData.push_back(m_pGeometry->positions[i].y);
        geometryData.push_back(m_pGeometry->positions[i].z);

        if (hasColor)
        {
            geometryData.push_back(m_pGeometry->colors[i].x);
            geometryData.push_back(m_pGeometry->colors[i].y);
            geometryData.push_back(m_pGeometry->colors[i].z);
            geometryData.push_back(m_pGeometry->colors[i].w);
        }

        if (hasUv)
        {
            geometryData.push_back(m_pGeometry->UVs[i].x);
            geometryData.push_back(m_pGeometry->UVs[i].y);
        }

        if (hasNormal)
        {
            geometryData.push_back(m_pGeometry->normals[i].x);
            geometryData.push_back(m_pGeometry->normals[i].y);
            geometryData.push_back(m_pGeometry->normals[i].z);
        }
    }

    if (deferred)
    {
        m_pendingVertices = geometryData;
        m_pendingIndices = m_pGeometry->indicies;
        m_pendingFloatStride = floatStride;
        m_vertexCount = static_cast<UINT>(vertexCount);
        m_indexCount = static_cast<UINT>(m_pGeometry->indicies.size());
    }
    else
    {
        UploadBuffers(
            geometryData.data(),
            static_cast<UINT>(vertexCount),
            m_pGeometry->indicies.data(),
            static_cast<UINT>(m_pGeometry->indicies.size()),
            static_cast<UINT>(floatStride)
        );
    }
}

void Mesh::UploadBuffers(float32* vertices, UINT vertexCount, uint32* indices, UINT indexCount, UINT floatStride)
{
    m_vertexCount = vertexCount;
    m_indexCount = indexCount;

    UINT vertexBufferSize = vertexCount * floatStride * sizeof(float32);
    UINT indexBufferSize = indexCount * sizeof(uint32);

    m_pVertexBufferGPU = m_pRender->GetRenderResources()->CreateDefaultBuffer(
        m_pRender->GetRenderResources()->GetDevice(),
        m_pRender->GetRenderResources()->GetCommandList(),
        vertices,
        vertexBufferSize,
        &m_pVertexBufferUploader,
        D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
    );

    m_pIndexBufferGPU = m_pRender->GetRenderResources()->CreateDefaultBuffer(
        m_pRender->GetRenderResources()->GetDevice(),
        m_pRender->GetRenderResources()->GetCommandList(),
        indices,
        indexBufferSize,
        &m_pIndexBufferUploader,
        D3D12_RESOURCE_STATE_INDEX_BUFFER
    );

    if(m_pIndexBufferUploader)
    {
        void* mapped = nullptr;
        D3D12_RANGE readRange = { 0, indexBufferSize };
        if (SUCCEEDED(m_pIndexBufferUploader->Map(0, &readRange, &mapped)))
        {
            uint32_t* idx = reinterpret_cast<uint32_t*>(mapped);
            D3D12_RANGE writtenRange = { 0, 0 };
            m_pIndexBufferUploader->Unmap(0, &writtenRange);
        }
        else
        {
            Utils::DebugError("uploader of IB is not mappable");
        }
    }
    else
    {
        Utils::DebugError("m_pIndexBufferUploader == null after CreateDefaultBuffer()");
    }

    m_vertexBuffer.BufferLocation = m_pVertexBufferGPU->GetGPUVirtualAddress();
    m_vertexBuffer.StrideInBytes = sizeof(float32) * floatStride;
    m_vertexBuffer.SizeInBytes = vertexBufferSize;

    m_indexBuffer.BufferLocation = m_pIndexBufferGPU->GetGPUVirtualAddress();
    m_indexBuffer.Format = DXGI_FORMAT_R32_UINT;
    m_indexBuffer.SizeInBytes = indexBufferSize;

    m_pVertexBufferUploader->SetName(L"VBufferUploader");
    m_pVertexBufferGPU->SetName(L"VBufferGPU");
    m_pIndexBufferUploader->SetName(L"IBufferUploader");
    m_pIndexBufferGPU->SetName(L"IBufferGPU");

}

void Mesh::UploadBuffersDeferred(ID3D12GraphicsCommandList* commandList)
{
    if (m_pendingVertices.empty() || m_pendingIndices.empty()) return;

    UploadBuffers(
        m_pendingVertices.data(),
        m_vertexCount,
        m_pendingIndices.data(),
        m_indexCount,
        m_pendingFloatStride
    );

    m_pendingVertices.clear();
    m_pendingIndices.clear();
}
