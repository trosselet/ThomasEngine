#include <Render/pch.h>
#include <Render/Header/Material.h>

#include <Render/Header/Render.h>
#include <Render/Header/RenderResources.h>
#include <Render/Header/PSOManager.h>
#include <Render/Header/Texture.h>

Material::Material(Render* pRender) :
	m_pRender(pRender),
	m_uploadBuffer(pRender->GetRenderResources()->GetDevice(), 1, 1),
	m_uploadMaterialBuffer(pRender->GetRenderResources()->GetDevice(), 1, 1),
	m_pDiffuseTexture(nullptr),
	m_pNormalTexture(nullptr),
	m_pSpecularTexture(nullptr)
{
	m_uploadBuffer.GetResource()->SetName(L"MaterialWorldUBuffer");
    m_uploadMaterialBuffer.GetResource()->SetName(L"MaterialPropertiesUBuffer");

    m_materialData = {};
	m_materialData.ambientColor = DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_materialData.diffuseColor = DirectX::XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	m_materialData.specularColor = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_materialData.shininess = 32.0f;
	m_materialData.roughness = 0.5f;
	m_materialData.metallic = 0.0f;
	m_materialData.emmisiveStrength = 0.0f;


	m_pPso = PSOManager::GetInstance()->GetPSO(L"shader.hlsl");
}

Material::~Material()
{
 	if (m_pDiffuseTexture && !m_isInCache)
	{
		delete m_pDiffuseTexture;
		m_pDiffuseTexture = nullptr;
	}
 	if (m_pNormalTexture && !m_isInCache)
	{
		delete m_pNormalTexture;
		m_pNormalTexture = nullptr;
	}
 	if (m_pSpecularTexture && !m_isInCache)
	{
		delete m_pSpecularTexture;
		m_pSpecularTexture = nullptr;
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

void Material::UpdateMaterialConstantBuffer()
{
	std::lock_guard<std::mutex> lock(m_uploadMutex);
	m_uploadMaterialBuffer.CopyData(0, m_materialData);
}

void Material::SetTexture(Texture* pTexture, bool fromCache)
{
	m_pDiffuseTexture = pTexture;
	m_isInCache = !fromCache;
}

void Material::SetNormalTexture(Texture* pTexture, bool fromCache)
{
	m_pNormalTexture = pTexture;
}

void Material::SetSpecularTexture(Texture* pTexture, bool fromCache)
{
	m_pSpecularTexture = pTexture;
}

bool Material::UpdateTexture(int16 materialPosition, int16 materialPropertiesPosition, ID3D12GraphicsCommandList* cmd)
{
    if (m_pDiffuseTexture == nullptr) return false;

    if (cmd == nullptr)
    {
        if (m_pRender)
            cmd = m_pRender->GetRenderResources()->GetCommandList();
        else
            return false;
    }

    std::lock_guard<std::mutex> lock(m_uploadMutex);

	cmd->SetGraphicsRootDescriptorTable(materialPosition, m_pDiffuseTexture->GetTextureAddress());

    cmd->SetGraphicsRootConstantBufferView((UINT)materialPropertiesPosition, m_uploadMaterialBuffer.GetResource()->GetGPUVirtualAddress());
    return true;
}

Texture* Material::GetTexture()
{
	return m_pDiffuseTexture;
}

void Material::SetMaterialData(const MaterialData& data)
{
	std::lock_guard<std::mutex> lock(m_uploadMutex);
	m_materialData = data;
}
