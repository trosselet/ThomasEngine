#ifndef UPLOAD_BUFFER_INCLUDE__H
#define UPLOAD_BUFFER_INCLUDE__H

#include "../Tools/Header/Utils.h"
#include <DirectXMath.h>

struct ConstantBuffer
{
};

struct CB
{
    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT4 color;
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