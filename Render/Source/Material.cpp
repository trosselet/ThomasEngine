#include <Render/pch.h>
#include <Render/Header/Material.h>

#include <Render/Header/Render.h>
#include <Render/Header/RenderResources.h>
#include <Render/Header/Texture.h>

Material::Material(Render* pRender) :
	m_pRender(pRender),
	m_uploadBuffer(pRender->GetRenderResources()->GetDevice(), 1, 1),
	m_pTexture(nullptr)
{
	m_uploadBuffer.GetResource()->SetName(L"MaterialUBuffer");
}

Material::~Material()
{
 	if (m_pTexture && !m_isInCache)
	{
		delete m_pTexture;
		m_pTexture = nullptr;
	}
}

UploadBuffer<ObjectData>* Material::GetUploadBuffer()
{
	return &m_uploadBuffer;
}

void Material::UpdateWorldConstantBuffer(DirectX::XMMATRIX const& matrix)
{
    ObjectData dataCb = {};
    DirectX::XMStoreFloat4x4(&dataCb.world, DirectX::XMMatrixTranspose(matrix));
    std::lock_guard<std::mutex> lock(m_uploadMutex);
    m_uploadBuffer.CopyData(0, dataCb);
}

void Material::SetTexture(Texture* pTexture, bool fromCache)
{
	m_pTexture = pTexture;
	m_isInCache = !fromCache;
}

bool Material::UpdateTexture(int16 position, ID3D12GraphicsCommandList* cmd)
{
    if (m_pTexture == nullptr) return false;

    // allow caller to provide command list for multi-threaded recording (bundle)
    if (cmd == nullptr)
    {
        if (m_pRender)
            cmd = m_pRender->GetRenderResources()->GetCommandList();
        else
            return false;
    }

    std::lock_guard<std::mutex> lock(m_uploadMutex);
    cmd->SetGraphicsRootDescriptorTable((UINT)position, m_pTexture->GetTextureAddress());
    return true;
}

Texture* Material::GetTexture()
{
	return m_pTexture;
}
