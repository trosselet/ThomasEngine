#ifndef MATERIAL_INCLUDE__H
#define MATERIAL_INCLUDE__H

#include <Render/Header/UploadBuffer.h>
#include <Tools/Header/PrimitiveTypes.h>
#include <mutex>

struct ObjMaterial;

struct PSOSettings;

class Render;
class Texture;

class Material
{
public:
	Material(Render* pRender, uint32 psoFlag);
	~Material();

	UploadBuffer<ObjectData>* GetUploadBuffer();
	void UpdateWorldConstantBuffer(DirectX::XMMATRIX const& matrix);
	void UpdateMaterialConstantBuffer();

	void SetTexture(Texture* pTexture, bool fromCache = true);
	void SetNormalTexture(Texture* pTexture, bool fromCache = true);
	void SetSpecularTexture(Texture* pTexture, bool fromCache = true);
	bool UpdateTexture(int16 materialPosition, int16 materialPropertiesPosition, ID3D12GraphicsCommandList* cmd = nullptr);
	Texture* GetTexture();

public:
	MaterialData& GetMaterialData() { return m_materialData; }
	void SetMaterialData(const MaterialData& data);
	void SetEmmisiveStrength(float emmisive) { m_materialData.emmisiveStrength = emmisive; }
	void SetAmbientColor(const DirectX::XMFLOAT4& color) { m_materialData.ambientColor = color; }
	void SetDiffuseColor(const DirectX::XMFLOAT4& color) { m_materialData.diffuseColor = color; }
	void SetSpecularColor(const DirectX::XMFLOAT4& color) { m_materialData.specularColor = color; }
	void SetShininess(float shininess) { m_materialData.shininess = shininess; }
	void SetRoughness(float roughness) { m_materialData.roughness = roughness; }
	void SetMetallic(float metallic) { m_materialData.metallic = metallic; }

public:
	void SetPSO(ID3D12PipelineState* pPso) { m_pPso = pPso; };
	ID3D12PipelineState* GetPSO() { return m_pPso; };

	static inline bool sSetWireframe = false;
private:
	UploadBuffer<ObjectData> m_uploadBuffer;
	UploadBuffer<MaterialData> m_uploadMaterialBuffer;

	MaterialData m_materialData;
	std::mutex m_uploadMutex;

	Texture* m_pDiffuseTexture;
	Texture* m_pNormalTexture;
	Texture* m_pSpecularTexture;

	Render* m_pRender;
	ID3D12PipelineState* m_pPso;

	bool m_isTextureWireframed = false;
	PSOSettings* m_pSettings = {};

	bool m_isInCache = false;
};

#endif // !MATERIAL_INCLUDE__H