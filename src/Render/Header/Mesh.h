#ifndef MESH_INCLUDE__H
#define MESH_INCLUDE__H

#include <d3d12.h>
#include <vector>


struct ID3D12Resource;
struct Geometry;

class Render;

class Mesh
{
public:
	Mesh(Geometry* pGeometry, Render* pRender, bool deferUpload = false);
    ~Mesh();

    D3D12_INDEX_BUFFER_VIEW GetIndexBuffer();
    D3D12_VERTEX_BUFFER_VIEW GetVertexBuffer();
    UINT GetIndexCount();
    void ReleaseUploadBuffers();

    void UploadBuffersDeferred(ID3D12GraphicsCommandList* commandList);

private:
    void UploadGeometry(bool deferred);
    void UploadBuffers(float32* vertices, UINT vertexCount, uint32* indices, UINT indexCount, UINT floatStride);

private:
    Render* m_pRender = nullptr;
    Geometry* m_pGeometry = nullptr;

    ID3D12Resource* m_pVertexBufferUploader = nullptr;
    ID3D12Resource* m_pIndexBufferUploader = nullptr;
    ID3D12Resource* m_pVertexBufferGPU = nullptr;
    ID3D12Resource* m_pIndexBufferGPU = nullptr;
    UINT m_vertexCount = 0;
    UINT m_indexCount = 0;

    D3D12_VERTEX_BUFFER_VIEW m_vertexBuffer = {};
    D3D12_INDEX_BUFFER_VIEW m_indexBuffer = {};

private:
    std::vector<float32> m_pendingVertices;
    std::vector<uint32> m_pendingIndices;
    UINT m_pendingFloatStride = 0;
};

#endif // !MESH_INCLUDE__H
