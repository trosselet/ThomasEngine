#include <Render/pch.h>
#include <Render/Header/GraphicEngine.h>
#include <Render/Header/RenderTarget.h>
#include <Render/Header/Render.h>
#include <Render/Header/RenderResources.h>
#include <Render/Header/Window.h>
#include <Render/Header/Mesh.h>
#include <Render/Header/Material.h>
#include <Render/Header/Texture.h>
#include <Render/Header/PrimitiveGeometry.h>
#include <Render/Header/Geometry.h>
#include <Render/Header/UploadBuffer.h>
#include <Render/Header/ObjFactory.h>
#include <Render/Header/FbxFactory.h>
#include <Render/Header/ObjModel.h>

GraphicEngine::GraphicEngine(const Window* pWindow)
{
    m_pRender = NEW Render(pWindow);

    RecreateOffscreenRT(pWindow->GetWidth(), pWindow->GetHeight());

    PrimitiveGeometry::InitializeGeometry();
}

GraphicEngine::~GraphicEngine()
{
    m_textureCache.Release();
    m_geometryCache.Release();
    m_objCache.Release();
    m_meshCache.Release();

    if (m_pRender)
    {
        delete m_pRender;
        m_pRender = nullptr;
    }
}

// ------------------- Frame -------------------

void GraphicEngine::BeginDraw()
{
    if (m_pRender)
        m_pRender->BeginFrame();
}

void GraphicEngine::RenderFrame(Mesh* pMesh, Material* pMaterial, DirectX::XMFLOAT4X4 const& objectWorldMatrix)
{
    if (m_pRender)
        m_pRender->Draw(pMesh, pMaterial, objectWorldMatrix);
}

void GraphicEngine::Display()
{
    if (m_pRender)
        m_pRender->EndFrame();
}

// ------------------- Geometry / Mesh -------------------

Geometry* GraphicEngine::CreatePrimitiveGeometry(PrimitiveGeometryType primitiveType, Color color)
{
    Geometry const* const pGeo = PrimitiveGeometry::GetPrimitiveGeometry(primitiveType);
    Geometry* pResult = NEW Geometry();
    *pResult = *pGeo;

    for (size_t i = 0; i < pResult->positions.size(); i++)
        pResult->colors.push_back(DirectX::XMFLOAT4(color.r, color.g, color.b, color.a));

    return pResult;
}

ObjModel* GraphicEngine::CreateGeometryFromFile(const char* meshPath, const char* extension, Color color)
{
    if (strcmp(extension, ".obj") == 0)
        return ObjFactory::LoadObjFile(meshPath, color);
    else if (strcmp(extension, ".fbx") == 0)
    {
        FbxParser::SceneData* pSceneData = FbxFactory::LoadFbxFile(meshPath);
        ObjModel* pObjModel = NEW ObjModel();

        for (const auto& mesh : pSceneData->meshes)
        {
            ObjSubMesh subMesh;
            const auto& mat = pSceneData->materials[mesh.materialIndex];

            subMesh.material.diffuseColor = Vector3{ mat.diffuseColor.data[0], mat.diffuseColor.data[1], mat.diffuseColor.data[2] };
            subMesh.material.specularColor = Vector3{ mat.specularColor.data[0], mat.specularColor.data[1], mat.specularColor.data[2] };
            subMesh.material.ambientColor = Vector3{ mat.ambientColor.data[0], mat.ambientColor.data[1], mat.ambientColor.data[2] };
            subMesh.material.shininess = mat.shininess;

            if (mat.textures.count("diffuse")) subMesh.material.diffuseTexturePath = mat.textures.at("diffuse");
            if (mat.textures.count("normals")) subMesh.material.normalTexturePath = mat.textures.at("normals");
            if (mat.textures.count("specular")) subMesh.material.specularTexturePath = mat.textures.at("specular");

            Geometry* geo = &subMesh.geometry;
            geo->positions.reserve(mesh.positions.size());
            for (const Vector3& p : mesh.positions)
                geo->positions.emplace_back(XMFLOAT3(p.data[0], p.data[1], p.data[2]));

            geo->normals.reserve(mesh.normals.size());
            for (const Vector3& n : mesh.normals)
                geo->normals.emplace_back(XMFLOAT3(n.data[0], n.data[1], n.data[2]));

            geo->UVs.reserve(mesh.uvs0.size());
            for (const Vector3& uv : mesh.uvs0)
                geo->UVs.emplace_back(XMFLOAT2(uv.data[0], uv.data[1]));

            geo->indicies = mesh.indices;
            geo->indexNumber = static_cast<uint32>(mesh.indices.size());

            pObjModel->subMeshes.push_back(subMesh);
        }

        delete pSceneData;
        return pObjModel;
    }

    return nullptr;
}

Mesh* GraphicEngine::CreateMesh(Geometry* pGeometry)
{
    m_pRender->GetRenderResources()->ResetCommandList();
    Mesh* mesh = NEW Mesh(pGeometry, m_pRender);
    m_pRender->GetRenderResources()->GetCommandList()->Close();
    m_pRender->GetRenderResources()->ExecuteCommandList();
    m_pRender->GetRenderResources()->FlushQueue();
    mesh->ReleaseUploadBuffers();
    return mesh;
}

Material* GraphicEngine::CreateMaterial(uint32 psoFlag)
{
    return NEW Material(m_pRender, psoFlag);
}

Texture* GraphicEngine::CreateTexture(std::string& filePath, const char* extension)
{
    m_pRender->GetRenderResources()->ResetCommandList();
    Texture* pTexture = nullptr;
    if (strcmp(extension, ".dds") == 0)
        pTexture = NEW Texture(filePath.c_str(), this);
    else
        pTexture = NEW Texture(filePath, this);

    m_pRender->GetRenderResources()->GetCommandList()->Close();
    m_pRender->GetRenderResources()->ExecuteCommandList();
    m_pRender->GetRenderResources()->FlushQueue();
    return pTexture;
}

void GraphicEngine::SetColor(Geometry* pGeometry, Color c)
{
    if (!pGeometry) return;
    pGeometry->colors.clear();
    for (size_t i = 0; i < pGeometry->positions.size(); i++)
        pGeometry->colors.push_back(DirectX::XMFLOAT4(c.r, c.g, c.b, c.a));
}

// ------------------- Camera -------------------

void GraphicEngine::UpdateCameraAt(Vector3 const& position, Vector3 const& target, Vector3 const& up,
    float32 viewWidth, float32 viewHeight, float32 fov, float32 cNear, float32 cFar,
    Matrix4x4& projectionMatrix, Matrix4x4& viewMatrix)
{
    DirectX::XMFLOAT3 d12Pos = DirectX::XMFLOAT3(position.ToXMFLOAT3());
    DirectX::XMFLOAT3 d12Target = DirectX::XMFLOAT3(target.ToXMFLOAT3());
    DirectX::XMFLOAT3 d12Up = DirectX::XMFLOAT3(up.ToXMFLOAT3());

    DirectX::XMVECTOR camPos = XMLoadFloat3(&d12Pos);
    DirectX::XMVECTOR camTarget = XMLoadFloat3(&d12Target);
    DirectX::XMVECTOR camUp = XMLoadFloat3(&d12Up);

    DirectX::XMMATRIX tempProj = DirectX::XMMatrixPerspectiveFovLH(fov, viewWidth / viewHeight, cNear, cFar);
    DirectX::XMMATRIX tempView = DirectX::XMMatrixLookAtLH(camPos, camTarget, camUp);

    projectionMatrix = DirectX::XMMATRIX(DirectX::XMMatrixTranspose(tempProj));
    viewMatrix = DirectX::XMMATRIX(DirectX::XMMatrixTranspose(tempView));

    CameraCB camera = {};
    DirectX::XMStoreFloat4x4(&camera.projectionMatrix, projectionMatrix);
    DirectX::XMStoreFloat4x4(&camera.viewMatrix, viewMatrix);

    m_pRender->GetCameraCB()->CopyData(0, camera);
}

void GraphicEngine::UpdateCameraTo(Vector3 const& position, Vector3 const& target, Vector3 const& up,
    float32 viewWidth, float32 viewHeight, float32 fov, float32 cNear, float32 cFar,
    Matrix4x4& projectionMatrix, Matrix4x4& viewMatrix)
{
    UpdateCameraAt(position, target, up, viewWidth, viewHeight, fov, cNear, cFar, projectionMatrix, viewMatrix);
}

void GraphicEngine::BindFrameConstants()
{
    ID3D12GraphicsCommandList* cmd = m_pRender->GetRenderResources()->GetCommandList();
    if (!cmd) return;
    cmd->SetGraphicsRootConstantBufferView(0, m_pRender->GetCameraCB()->GetResource()->GetGPUVirtualAddress());
}

UINT64 GraphicEngine::GetFrameCBAddress() const
{
    return m_pRender ? m_pRender->GetCameraCB()->GetResource()->GetGPUVirtualAddress() : 0;
}

// ------------------- Deferred Mesh Uploads -------------------

Mesh* GraphicEngine::CreateMeshDeferred(Geometry* pGeometry)
{
    Mesh* mesh = NEW Mesh(pGeometry, m_pRender, true);
    m_pendingMeshUploads.push_back(mesh);
    return mesh;
}

void GraphicEngine::ProcessPendingUploads()
{
    if (m_pendingMeshUploads.empty()) return;

    auto* res = m_pRender->GetRenderResources();
    res->ResetCommandList();

    for (Mesh* mesh : m_pendingMeshUploads)
        mesh->UploadBuffersDeferred(res->GetCommandList());

    res->GetCommandList()->Close();
    res->ExecuteCommandList();
    res->FlushQueue();

    for (Mesh* mesh : m_pendingMeshUploads)
        mesh->ReleaseUploadBuffers();

    m_pendingMeshUploads.clear();
}

// ------------------- RenderTarget access -------------------

Render* GraphicEngine::GetRender() 
{
    return m_pRender;
}

RenderTarget* GraphicEngine::GetMainRenderTarget() const
{
    return m_pOffscreenRT;
}

std::vector<RenderTarget*> GraphicEngine::GetRenderTargets() const
{
    return { m_pOffscreenRT };
}


// ------------------- Offscreen RT -------------------

void GraphicEngine::RecreateOffscreenRT(uint32 width, uint32 height)
{
    if (!m_pRender) return;

    WindowResizeInfo resize = {};
    resize.width = width;
    resize.height = height;

    m_pRender->RequestResizeRT(resize);
    m_pOffscreenRT = m_pRender->GetOffscreenRenderTarget();
}


// ------------------- Wireframe -------------------

void GraphicEngine::SetWireframe(bool wireframe)
{
    if (m_pRender)
        m_pRender->SetWireframe(wireframe);
}
