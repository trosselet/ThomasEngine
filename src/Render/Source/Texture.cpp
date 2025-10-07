#include "pch.h"
#include "Header/Texture.h"

#include "Header/DDSTextureLoader.h"
#include "Header/GraphicEngine.h"
#include "Header/Render.h"
#include "Header/RenderResources.h"

Texture::Texture(int8 const* path, GraphicEngine* pGraphic) : m_pTexture(nullptr), m_pTextureUploadHeap(nullptr)
{
    std::string filePath = "../../res/Gameplay/textures/" + std::string(reinterpret_cast<const char*>(path));
    std::wstring wPath(filePath.begin(), filePath.end());

    HRESULT hr = DirectX::CreateDDSTextureFromFile12(
        pGraphic->GetRender()->GetRenderResources()->GetDevice(),
        pGraphic->GetRender()->GetRenderResources()->GetCommandList(),
        wPath.c_str(),
        &m_pTexture,
        &m_pTextureUploadHeap,
        m_width,
        m_height
    );

    if (SUCCEEDED(hr))
    {
        m_pTexture->SetName(L"Texture Resource");
        m_pTextureUploadHeap->SetName(L"Texture Resource Upload Heap");

        UINT descriptorIndex = pGraphic->GetRender()->GetRenderResources()->AllocateSRVHeapIndex();

        UINT size = pGraphic->GetRender()->GetRenderResources()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

        D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle = pGraphic->GetRender()->GetRenderResources()->GetCbvSrvUavDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
        descriptorHandle.ptr += static_cast<SIZE_T>(descriptorIndex) * size;

        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Format = m_pTexture->GetDesc().Format;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.MipLevels = m_pTexture->GetDesc().MipLevels;
        srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

        D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = pGraphic->GetRender()->GetRenderResources()->GetCbvSrvUavDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
        gpuHandle.ptr += static_cast<SIZE_T>(descriptorIndex) * size;

        pGraphic->GetRender()->GetRenderResources()->GetDevice()->CreateShaderResourceView(
            m_pTexture, &srvDesc, descriptorHandle);

        m_textureAddress = gpuHandle;
    }
    else
    {
        Utils::DebugError("Texture can't be created!!!!");
    }
    

}

Texture::~Texture()
{
    if (m_pTextureUploadHeap)
    {
        m_pTextureUploadHeap->Release();
        m_pTextureUploadHeap = nullptr;
    }

    if (m_pTexture)
    {
        m_pTexture->Release();
        m_pTexture = nullptr;
    }
}

D3D12_GPU_DESCRIPTOR_HANDLE Texture::GetTextureAddress()
{
    return m_textureAddress;
}
