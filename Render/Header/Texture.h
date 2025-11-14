#ifndef TEXTURE_INCLUDE__H
#define TEXTURE_INCLUDE__H

#include <Tools/Header/PrimitiveTypes.h>

class GraphicEngine;

class Texture
{
public:

    Texture(int8 const* path, GraphicEngine* pGraphic);
    ~Texture();

    D3D12_GPU_DESCRIPTOR_HANDLE GetTextureAddress();

    const char* GetTexturePath();

private:
    ID3D12Resource* m_pTexture;
    ID3D12Resource* m_pTextureUploadHeap;

    D3D12_DESCRIPTOR_HEAP_DESC m_srvHeapDesc{};
    D3D12_GPU_DESCRIPTOR_HANDLE m_textureAddress{};

    int m_width = 0;
    int m_height = 0;

    const char* m_path = "";

};


#endif // !TEXTURE_INCLUDE__H
