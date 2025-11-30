#include <Render/pch.h>

#include <Render/Header/GraphicEngine.h>
#include <Render/Header/Window.h>
#include <Render/Header/Render.h>
#include <Render/Header/RenderTarget.h>

#include <Render/Header/PrimitiveGeometry.h>
#include <Render/Header/Geometry.h>


#include <Render/Header/RenderResources.h>
#include <Render/Header/Mesh.h>
#include <Render/Header/Material.h>
#include <Render/Header/Texture.h>
#include <Render/Header/ObjFactory.h>
#include <Render/Header/FbxFactory.h>
#include <Render/Header/UploadBuffer.h>

GraphicEngine::GraphicEngine(const Window* pWindow)
{
	m_pRender = new Render(pWindow);
    //m_pOffscreenRT = new RenderTarget(m_pRender->GetRenderResources(), pWindow->GetWidth(), pWindow->GetHeight());
    //m_pRender->SetOffscreenRenderTarget(m_pOffscreenRT);
	PrimitiveGeometry::InitializeGeometry();
}

UINT64 GraphicEngine::GetFrameCBAddress() const
{
    if (!m_pRender) return 0;
    return m_pRender->m_pCbCurrentViewProjInstance->GetResource()->GetGPUVirtualAddress();
}

void GraphicEngine::BindFrameConstants()
{
    Render* r = m_pRender;
    if (!r) return;
    ID3D12GraphicsCommandList* cmd = r->GetRenderResources()->GetCommandList();
    if (!cmd) return;
    cmd->SetGraphicsRootConstantBufferView(0, r->m_pCbCurrentViewProjInstance->GetResource()->GetGPUVirtualAddress());
}

GraphicEngine::~GraphicEngine()
{

	m_textureCache.Release();
	m_geometryCache.Release();
	m_meshCache.Release();

	delete m_pRender;

    if (m_pOffscreenRT)
    {
        delete m_pOffscreenRT;
        m_pOffscreenRT = nullptr;
    }
}

void GraphicEngine::BeginDraw()
{
	m_pRender->Clear();
}

void GraphicEngine::RenderFrame(Mesh* pMesh, Material* pMaterial, DirectX::XMFLOAT4X4 const& objectWorldMatrix)
{
	m_pRender->Draw(pMesh, pMaterial, objectWorldMatrix);
}

void GraphicEngine::Display()
{
	m_pRender->Display();
}

Geometry* GraphicEngine::CreatePrimitiveGeometry(PrimitiveGeometryType primitiveType, Color color)
{
	Geometry const* const pGeo = PrimitiveGeometry::GetPrimitiveGeometry(primitiveType);
	Geometry* pResult = new Geometry();
	*pResult = *pGeo;

	for (int i = 0; i < pResult->positions.size(); i++)
	{
		pResult->colors.push_back(DirectX::XMFLOAT4(color.r, color.g, color.b, color.a));
	}

	return pResult;
}

Geometry* GraphicEngine::CreateGeometryFromFile(const char* meshPath, const char* extension, Color color)
{
	if (strcmp(extension, ".obj") == 0)
		return ObjFactory::LoadObjFile(meshPath, color);
	/*else if (strcmp(extension, ".fbx") == 0)
		return FbxFactory::LoadFbxFile(meshPath, color);*/

	return nullptr;
}

Mesh* GraphicEngine::CreateMesh(Geometry* pGeometry)
{
	m_pRender->GetRenderResources()->ResetCommandList();
	Mesh* mesh = new Mesh(pGeometry, m_pRender);
	m_pRender->GetRenderResources()->GetCommandList()->Close();
	m_pRender->GetRenderResources()->ExecuteCommandList();
	m_pRender->GetRenderResources()->FlushQueue();
	mesh->ReleaseUploadBuffers();
	return mesh;
}

Material* GraphicEngine::CreateMaterial()
{
	return new Material(m_pRender);
}

Texture* GraphicEngine::CreateTexture(char const* filePath)
{
	m_pRender->GetRenderResources()->ResetCommandList();
	Texture* pTexture = new Texture(filePath, this);
	m_pRender->GetRenderResources()->GetCommandList()->Close();
	m_pRender->GetRenderResources()->ExecuteCommandList();
	m_pRender->GetRenderResources()->FlushQueue();
	return pTexture;
}

void GraphicEngine::SetColor(Geometry* pGeometry, Color c)
{
	if (pGeometry != nullptr)
	{
		pGeometry->colors.clear();
	}
	else
	{
		return;
	}

	for (int i = 0; i < pGeometry->positions.size(); i++)
	{
		pGeometry->colors.push_back(DirectX::XMFLOAT4(c.r, c.g, c.b, c.a));
	}
}

void GraphicEngine::UpdateCameraAt(Vector3 const& position, Vector3 const& target, Vector3 const& up, float32 viewWidth, float32 viewHeight, float32 fov, float32 cNear, float32 cFar, Matrix4x4& projectionMatrix, Matrix4x4& viewMatrix)
{
	DirectX::XMFLOAT3 d12Position = DirectX::XMFLOAT3(position.ToXMFLOAT3());
	DirectX::XMFLOAT3 d12Target = DirectX::XMFLOAT3(target.ToXMFLOAT3());
	DirectX::XMFLOAT3 d12Up = DirectX::XMFLOAT3(up.ToXMFLOAT3());

	DirectX::XMVECTOR camPos = XMLoadFloat3(&d12Position);
	DirectX::XMVECTOR camTarget = XMLoadFloat3(&d12Target);
	DirectX::XMVECTOR camUp = XMLoadFloat3(&d12Up);

	DirectX::XMMATRIX tempProj = DirectX::XMMatrixPerspectiveFovLH(fov, viewWidth / viewHeight, cNear, cFar);
	DirectX::XMMATRIX tempView = DirectX::XMMatrixLookAtLH(camPos, camTarget, camUp);

	projectionMatrix = DirectX::XMMATRIX(DirectX::XMMatrixTranspose(tempProj));
	viewMatrix = DirectX::XMMATRIX(DirectX::XMMatrixTranspose(tempView));

	CameraCB camera = {};
	DirectX::XMStoreFloat4x4(&camera.projectionMatrix, projectionMatrix);
	DirectX::XMStoreFloat4x4(&camera.viewMatrix, viewMatrix);

	m_pRender->m_pCbCurrentViewProjInstance->CopyData(0, camera);
}

void GraphicEngine::UpdateCameraTo(Vector3 const& position, Vector3 const& target, Vector3 const& up, float32 viewWidth, float32 viewHeight, float32 fov, float32 cNear, float32 cFar, Matrix4x4& projectionMatrix, Matrix4x4& viewMatrix)
{
	DirectX::XMFLOAT3 d12Position = DirectX::XMFLOAT3(position.ToXMFLOAT3());
	DirectX::XMFLOAT3 d12Target = DirectX::XMFLOAT3(target.ToXMFLOAT3());
	DirectX::XMFLOAT3 d12Up = DirectX::XMFLOAT3(up.ToXMFLOAT3());

	DirectX::XMVECTOR camPos = XMLoadFloat3(&d12Position);
	DirectX::XMVECTOR camTarget = XMLoadFloat3(&d12Target);
	DirectX::XMVECTOR camUp = XMLoadFloat3(&d12Up);

	DirectX::XMMATRIX tempProj = DirectX::XMMatrixPerspectiveFovLH(fov, viewWidth / viewHeight, cNear, cFar);
	DirectX::XMMATRIX tempView = DirectX::XMMatrixLookAtLH(camPos, camTarget, camUp);

	projectionMatrix = DirectX::XMMATRIX(DirectX::XMMatrixTranspose(tempProj));
	viewMatrix = DirectX::XMMATRIX(DirectX::XMMatrixTranspose(tempView));

	CameraCB camera = {};
	DirectX::XMStoreFloat4x4(&camera.projectionMatrix, projectionMatrix);
	DirectX::XMStoreFloat4x4(&camera.viewMatrix, viewMatrix);

	m_pRender->m_pCbCurrentViewProjInstance->CopyData(0, camera);
}

Mesh* GraphicEngine::CreateMeshDeferred(Geometry* pGeometry)
{
	Mesh* mesh = new Mesh(pGeometry, m_pRender, true);
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

Render* GraphicEngine::GetRender()
{
	return m_pRender;
}

void GraphicEngine::RecreateOffscreenRT(uint32 width, uint32 height)
{
    uint32 rtW = std::max<uint32>(1, width / 2);
    uint32 rtH = std::max<uint32>(1, height / 2);

    if (m_pOffscreenRT)
    {
        delete m_pOffscreenRT;
        m_pOffscreenRT = nullptr;
    }

    m_pOffscreenRT = new RenderTarget(m_pRender->GetRenderResources(), rtW, rtH);
    m_pRender->SetOffscreenRenderTarget(m_pOffscreenRT);
}
