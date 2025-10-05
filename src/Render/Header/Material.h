#ifndef MATERIAL_INCLUDE__H
#define MATERIAL_INCLUDE__H

#include "../Render/Header/UploadBuffer.h"

class Render;

class Material
{
public:
	Material(Render* pRender);
	~Material();

	void Release();

	UploadBuffer<ObjectData>* GetUploadBuffer();
	void UpdateWorldConstantBuffer(DirectX::XMMATRIX const& matrix);

private:
	UploadBuffer<ObjectData> m_uploadBuffer;
};

#endif // !MATERIAL_INCLUDE__H