#ifndef UPLOAD_BUFFER_INCLUDE__H
#define UPLOAD_BUFFER_INCLUDE__H

#include <Tools/Header/Utils.h>
#include <DirectXMath.h> 

#include <d3d12.h>
#include <wrl/client.h>
#include <cstring>

#include <Render/Header/ObjModel.h>

struct ConstantBuffer
{
};

struct CB
{
    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT4 color;
	int padding[1];
};

struct CameraCB : ConstantBuffer
{
    DirectX::XMFLOAT4X4 viewMatrix;
    DirectX::XMFLOAT4X4 projectionMatrix;
};

struct ObjectData : ConstantBuffer
{
    DirectX::XMFLOAT4X4 world;
};

struct MaterialData : ConstantBuffer
{
	DirectX::XMFLOAT4 ambientColor;
	DirectX::XMFLOAT4 diffuseColor;
	DirectX::XMFLOAT4 specularColor;
	float shininess;
	float roughness;
	float metallic;
	float emmisiveStrength;

    static MaterialData FromObjMaterial(const ObjMaterial& mat)
    {
        MaterialData data = {};
        data.ambientColor = DirectX::XMFLOAT4(mat.ambientColor.r, mat.ambientColor.g, mat.ambientColor.b, mat.opacity);
        data.diffuseColor = DirectX::XMFLOAT4(mat.diffuseColor.r, mat.diffuseColor.g, mat.diffuseColor.b, mat.opacity);
        data.specularColor = DirectX::XMFLOAT4(mat.specularColor.r, mat.specularColor.g, mat.specularColor.b, mat.opacity);
        data.shininess = mat.shininess;
        data.roughness = 1.0f - (mat.shininess / 256.0f);
        data.metallic = 0.0f;
        data.emmisiveStrength = 0.0f;
		return data;
    }

};

template<typename T>
class UploadBuffer
{
public:

    inline UploadBuffer(ID3D12Device* pDevice, UINT elementCount, bool isConstantBuffer)
        : m_pUploadBuffer(nullptr), m_pMappedData(nullptr), m_elementCount(elementCount)
    {
        m_elementByteSize = sizeof(T);
        if (isConstantBuffer)
        {
            m_elementByteSize = (m_elementByteSize + 255) & ~255;
        }

        D3D12_HEAP_PROPERTIES heapProps = {};
        heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
        heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

        D3D12_RESOURCE_DESC bufferDesc = {};
        bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        bufferDesc.Width = m_elementByteSize * elementCount;
        bufferDesc.Height = 1;
        bufferDesc.DepthOrArraySize = 1;
        bufferDesc.MipLevels = 1;
        bufferDesc.SampleDesc.Count = 1;
        bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

        if (pDevice->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_pUploadBuffer)) != S_OK)
        {
            Utils::DebugError(" Failed to create upload buffer.");
        }

        if (m_pUploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&m_pMappedData)) != S_OK)
        {
            Utils::DebugError(" Failed to map upload buffer.");
        }

    }

    inline ~UploadBuffer()
    {
        if (m_pUploadBuffer)
        {
            m_pUploadBuffer->Unmap(0, nullptr);
            m_pUploadBuffer->Release();
        }
        m_pMappedData = nullptr;
    }

    inline void CopyData(UINT elementIndex, const T& data)
    {
        memcpy(m_pMappedData + elementIndex * m_elementByteSize, &data, sizeof(T));
    }

    inline ID3D12Resource* GetResource() const { return m_pUploadBuffer; }
    inline UINT GetElementByteSize() const { return m_elementByteSize; }

private:
    ID3D12Resource* m_pUploadBuffer;
    BYTE* m_pMappedData;
    UINT m_elementByteSize;
    UINT m_elementCount;
};


#endif // !UPLOAD_BUFFER__H