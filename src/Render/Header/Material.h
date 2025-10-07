#ifndef MATERIAL_INCLUDE__H
#define MATERIAL_INCLUDE__H

#include "../Render/Header/UploadBuffer.h"

class Render;
class Texture;

class Material
{
public:
	Material(Render* pRender);
	~Material();

	void Release();

	UploadBuffer<ObjectData>* GetUploadBuffer();
	void UpdateWorldConstantBuffer(DirectX::XMMATRIX const& matrix);

	void SetTexture(Texture* pTexture);
	bool UpdateTexture(int16 position);
	Texture* GetTexture();

private:
	UploadBuffer<ObjectData> m_uploadBuffer;

	Texture* m_pTexture;
	Render* m_pRender;
};

#endif // !MATERIAL_INCLUDE__H