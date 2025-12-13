#include <Render/pch.h>
#include <Render/Header/Texture.h>

#include <Render/Header/DDSTextureLoader.h>
#include <Render/Header/GraphicEngine.h>
#include <Render/Header/Render.h>
#include <Render/Header/RenderResources.h>

#define STB_IMAGE_IMPLEMENTATION
#include <Render/ext/stb_image.h>

#if _EXE

#define TEXTURE_PATH "textures/"

#else

#define TEXTURE_PATH "textures/"

#endif

Texture::Texture(int8 const* path, GraphicEngine* pGraphic) : m_pTexture(nullptr), m_pTextureUploadHeap(nullptr), m_path(path)
{
    std::string filePath = TEXTURE_PATH + std::string(reinterpret_cast<const char*>(path));
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

Texture::Texture(std::string& path, GraphicEngine* pGraphic) : m_pTexture(nullptr), m_pTextureUploadHeap(nullptr), m_path(path)
{
    stbi_set_flip_vertically_on_load(false);

	std::string filePath = TEXTURE_PATH + path;
    uint8* data = stbi_load(filePath.c_str(), &m_width, &m_height, &m_channels, 4);
    if (!data)
    {
        Utils::DebugError("Failed to load image: ", path.c_str());

		std::string errorPath = TEXTURE_PATH + std::string("DefaultTex.png");

        data = stbi_load(errorPath.c_str(), &m_width, &m_height, &m_channels, 4);
    }

    m_channels = 4;
    CreateTextureFromMemory(data, m_width, m_height, m_channels, pGraphic);

    stbi_image_free(data);
}

Texture::~Texture()
{
    if (m_width >= 0 && m_height >= 0)
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
}

D3D12_GPU_DESCRIPTOR_HANDLE Texture::GetTextureAddress()
{
    return m_textureAddress;
}

const char* Texture::GetTexturePath()
{
    return m_path.c_str();
}

void Texture::CreateTextureFromMemory(uint8* pixels, int width, int height, int channels, GraphicEngine* pGraphic)
{
    ID3D12Device* device = pGraphic->GetRender()->GetRenderResources()->GetDevice();
    ID3D12GraphicsCommandList* cmdList = pGraphic->GetRender()->GetRenderResources()->GetCommandList();

    UINT alignedWidth = static_cast<UINT>(width);
    UINT alignedHeight = static_cast<UINT>(height);

    D3D12_RESOURCE_DESC texDesc = {};
    texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    texDesc.Alignment = 0;
    texDesc.Width = alignedWidth;
    texDesc.Height = alignedHeight;
    texDesc.DepthOrArraySize = 1;
    texDesc.MipLevels = 1;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    texDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

    D3D12_HEAP_PROPERTIES heapProps = {};
    heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

    D3D12_SUBRESOURCE_DATA subresource = {};
    subresource.pData = pixels;
    subresource.RowPitch = width * 4;
    subresource.SlicePitch = subresource.RowPitch * height;

    HRESULT hr = device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &texDesc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&m_pTexture)
    );

    if (FAILED(hr))
    {
        Utils::DebugError("Failed to create texture resource.");
        return;
    }

    const UINT64 uploadBufferSize = GetRequiredIntermediateSize(m_pTexture, 0, 1);

    D3D12_HEAP_PROPERTIES uploadHeapProps = {};
    uploadHeapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

    D3D12_RESOURCE_DESC uploadDesc = {};
    uploadDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    uploadDesc.Width = uploadBufferSize;
    uploadDesc.Height = 1;
    uploadDesc.DepthOrArraySize = 1;
    uploadDesc.MipLevels = 1;
    uploadDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	uploadDesc.SampleDesc.Count = 1;
	uploadDesc.SampleDesc.Quality = 0;

    hr = device->CreateCommittedResource(
        &uploadHeapProps,
        D3D12_HEAP_FLAG_NONE,
        &uploadDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_pTextureUploadHeap)
    );

    if (FAILED(hr))
    {
        Utils::DebugError("Failed to create upload heap.");
        return;
    }

    UpdateSubresources(cmdList, m_pTexture, m_pTextureUploadHeap, 0, 0, 1, &subresource);

    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = m_pTexture;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    cmdList->ResourceBarrier(1, &barrier);

    UINT descriptorIndex = pGraphic->GetRender()->GetRenderResources()->AllocateSRVHeapIndex();
    UINT size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = pGraphic->GetRender()->GetRenderResources()->GetCbvSrvUavDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
    cpuHandle.ptr += static_cast<SIZE_T>(descriptorIndex) * size;

    D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = pGraphic->GetRender()->GetRenderResources()->GetCbvSrvUavDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
    gpuHandle.ptr += static_cast<SIZE_T>(descriptorIndex) * size;

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Texture2D.MostDetailedMip = 0;

    device->CreateShaderResourceView(m_pTexture, &srvDesc, cpuHandle);
    m_textureAddress = gpuHandle;
}
