#include "pch.h"
#include "Header/Mesh.h"
#include "Header/Geometry.h"
#include "Header/Render.h"
#include "Header/UploadBuffer.h"
#include "Header/RenderResources.h"

Mesh::Mesh(Geometry* pGeometry, Render* pRender)
{
	m_pGeometry = pGeometry;
	m_pRender = pRender;
	UploadGeometry();
}

Mesh::~Mesh()
{
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

void Mesh::UploadGeometry()
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
    if (hasColor && m_pGeometry->colors.size() != vertexCount) { Utils::DebugError("Color count mismatch"); return; }
    if (hasUv && m_pGeometry->UVs.size() != vertexCount) { Utils::DebugError("UV count mismatch"); return; }
    if (hasNormal && m_pGeometry->normals.size() != vertexCount) { Utils::DebugError("Normal count mismatch"); return; }

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

    for (size_t i = 0; i < m_pGeometry->indicies.size(); ++i) {
        Utils::DebugLog("Index[", (int)i, "] ", m_pGeometry->indicies[i]);
    }


    UploadBuffers(geometryData.data(),
        static_cast<UINT>(m_pGeometry->positions.size()),
        m_pGeometry->indicies.data(),
        static_cast<UINT>(m_pGeometry->indicies.size()),
        static_cast<UINT>(floatStride));
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
            Utils::DebugLog(">>> IB Upload contents (first 6): ",
                idx[0], idx[1], idx[2], idx[3], idx[4], idx[5]);
            D3D12_RANGE writtenRange = { 0, 0 };
            m_pIndexBufferUploader->Unmap(0, &writtenRange);
        }
        else
        {
            Utils::DebugError("Impossible de mapper l'uploader d'IB");
        }
    }
    else
    {
        Utils::DebugError("m_pIndexBufferUploader == null juste après CreateDefaultBuffer()");
    }

    m_vertexBuffer.BufferLocation = m_pVertexBufferGPU->GetGPUVirtualAddress();
    m_vertexBuffer.StrideInBytes = sizeof(float32) * floatStride;
    m_vertexBuffer.SizeInBytes = vertexBufferSize;

    m_indexBuffer.BufferLocation = m_pIndexBufferGPU->GetGPUVirtualAddress();
    m_indexBuffer.Format = DXGI_FORMAT_R32_UINT;
    m_indexBuffer.SizeInBytes = indexBufferSize;

}
