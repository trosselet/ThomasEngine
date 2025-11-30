#ifndef MATERIAL_INCLUDE__H
#define MATERIAL_INCLUDE__H

#include <Render/Header/UploadBuffer.h>
#include <mutex>

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

	void SetTexture(Texture* pTexture, bool fromCache = true);
	bool UpdateTexture(int16 position, ID3D12GraphicsCommandList* cmd = nullptr);
	Texture* GetTexture();

private:
	UploadBuffer<ObjectData> m_uploadBuffer;
	std::mutex m_uploadMutex;

	Texture* m_pTexture;
	Render* m_pRender;

	bool m_isInCache = false;
};

#endif // !MATERIAL_INCLUDE__H