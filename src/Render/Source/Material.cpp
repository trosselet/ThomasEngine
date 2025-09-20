#include "pch.h"
#include "Header/Material.h"

#include "Header/Render.h"
#include "Header/RenderResources.h"

Material::Material(Render* pRender) :
	m_uploadBuffer(pRender->GetRenderResources()->GetDevice(), 1, 1)
{

}

Material::~Material()
{
}

UploadBuffer<ObjectData>* Material::GetUploadBuffer()
{
	return &m_uploadBuffer;
}

void Material::UpdateWorldConstantBuffer(DirectX::XMMATRIX const& matrix)
{
	ObjectData dataCb = {};
	DirectX::XMStoreFloat4x4(&dataCb.world, DirectX::XMMatrixTranspose(matrix));
	m_uploadBuffer.CopyData(0, dataCb);
}
