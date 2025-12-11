#include <Engine/pch.h>
#include <Engine/Header/MeshRenderer.h>
#include <Engine/Header/RenderSystem.h>
#include <Engine/Header/GameManager.h>
#include <Engine/Header/GameObject.h>
#include <Engine/Header/Scene.h>

#include <Render/Header/Geometry.h>
#include <Render/Header/PrimitiveGeometry.h>
#include <Render/Header/Mesh.h>
#include <Render/Header/Material.h>
#include <Render/Header/GraphicEngine.h>
#include <Render/Header/Window.h>
#include <Render/Header/ObjModel.h>


#include <Tools/Header/Color.h>
#include <filesystem>

MeshRenderer::~MeshRenderer()
{
	Free();
}

void MeshRenderer::SetRectangle(Color c)
{
	SetRectangle("DefaultTex.png", c);
}

void MeshRenderer::SetRectangle(const char* texturePath, Color c)
{
	Free();

	GraphicEngine& graphics = *GameManager::GetWindow().GetGraphicEngine();


	std::string geomKey = "primitive:rect";
	m_pGeometry = graphics.m_geometryCache.GetOrLoad(geomKey, [&]()->Geometry*
		{
			return graphics.CreatePrimitiveGeometry(SQUARE, c);
		});
	m_ownsGeometry = false;

	std::string textureExtension = std::filesystem::path(texturePath).extension().string();
	std::string texPath = std::string(texturePath);
	std::string texKey = std::string("tex:") + texturePath;
	Texture* pTexture = graphics.m_textureCache.GetOrLoad(texKey, [&]()->Texture*
		{
			return graphics.CreateTexture(texPath, textureExtension.c_str());
		});
	m_ownsMaterial = false;

	std::string meshKey = geomKey + std::string("_mesh");
	m_pMesh = graphics.m_meshCache.GetOrLoad(meshKey, [&]()->Mesh*
		{
			return graphics.CreateMeshDeferred(m_pGeometry);
		});
	m_ownsMesh = false;

	m_pMaterial = graphics.CreateMaterial();
	m_pMaterial->SetTexture(pTexture, m_ownsMaterial);

	m_primitive = true;
}

void MeshRenderer::SetCircle(Color c)
{
	SetCircle("DefaultTex.png", c);
}

void MeshRenderer::SetCircle(const char* texturePath, Color c)
{
	Free();

	GraphicEngine& graphics = *GameManager::GetWindow().GetGraphicEngine();

	std::string geomKey = "primitive:circle";
	m_pGeometry = graphics.m_geometryCache.GetOrLoad(geomKey, [&]()->Geometry*
		{
			return graphics.CreatePrimitiveGeometry(CIRCLE, c);
		});
	m_ownsGeometry = false;

	std::string textureExtension = std::filesystem::path(texturePath).extension().string();
	std::string texPath = std::string(texturePath);
	std::string texKey = std::string("tex:") + texturePath;
	Texture* pTexture = graphics.m_textureCache.GetOrLoad(texKey, [&]()->Texture*
		{
			return graphics.CreateTexture(texPath, textureExtension.c_str());
		});
	m_ownsMaterial = false;

	std::string meshKey = geomKey + std::string("_mesh");
	m_pMesh = graphics.m_meshCache.GetOrLoad(meshKey, [&]()->Mesh*
		{
			return graphics.CreateMeshDeferred(m_pGeometry);
		});
	m_ownsMesh = false;

	m_pMaterial = graphics.CreateMaterial();
	m_pMaterial->SetTexture(m_pTexture, m_ownsMaterial);
	m_primitive = true;
}

void MeshRenderer::SetCube(Color c)
{
	SetCube("DefaultTex.png", c);
}

void MeshRenderer::SetCube(const char* texturePath, Color c)
{
	Free();

	GraphicEngine& graphics = *GameManager::GetWindow().GetGraphicEngine();


	std::string geomKey = "primitive:cube";
	m_pGeometry = graphics.m_geometryCache.GetOrLoad(geomKey, [&]()->Geometry*
		{
			return graphics.CreatePrimitiveGeometry(CUBE, c);
		});
	m_ownsGeometry = false;

	std::string textureExtension = std::filesystem::path(texturePath).extension().string();
	std::string texPath = std::string(texturePath);
	std::string texKey = std::string("tex:") + texturePath;
	Texture* pTexture = graphics.m_textureCache.GetOrLoad(texKey, [&]()->Texture*
		{
			return graphics.CreateTexture(texPath, textureExtension.c_str());
		});
	m_ownsMaterial = false;

	std::string meshKey = geomKey + std::string("_mesh");
	m_pMesh = graphics.m_meshCache.GetOrLoad(meshKey, [&]()->Mesh*
		{
			return graphics.CreateMeshDeferred(m_pGeometry);
		});
	m_ownsMesh = false;

	m_pMaterial = graphics.CreateMaterial();
	m_pMaterial->SetTexture(pTexture, m_ownsMaterial);

	m_primitive = true;
}

void MeshRenderer::SetMeshFile(const char* objPath)
{
	SetMeshFileInternal(objPath, "DefaultTex.png");
}

void MeshRenderer::SetMeshFile(const char* objPath, const char* texturePath)
{
	SetMeshFileInternal(objPath, texturePath);
}

void MeshRenderer::SetMeshFileInternal(const char* objPath, const char* defaultTexturePath) 
{
	Free();

	GraphicEngine& graphics = *GameManager::GetWindow().GetGraphicEngine();
	std::string extension = std::filesystem::path(objPath).extension().string();
	std::string geomKey = extension + ":" + objPath;
	ObjModel* pObjModel = graphics.m_objCache.GetOrLoad(geomKey, [&]() -> ObjModel* 
		{ 
			return graphics.CreateGeometryFromFile(objPath, extension.c_str()); 
		}
	);

	m_ownsGeometry = false;

	if (!pObjModel || pObjModel->subMeshes.empty())
		return;

	GameObject* pParentObj = m_pOwner;
	ObjSubMesh& firstSubMesh = pObjModel->subMeshes[0];
	
	m_pGeometry = firstSubMesh.geometry;
	
	Texture* pTexture = nullptr;
	
	if (!firstSubMesh.material.diffuseTexturePath.empty())
	{ 
		std::string texPath = firstSubMesh.material.diffuseTexturePath;
		std::string texExt = std::filesystem::path(texPath).extension().string();
		std::string texKey = "tex:" + texPath;
		pTexture = graphics.m_textureCache.GetOrLoad(texKey, [&]() -> Texture* 
			{ 
				return graphics.CreateTexture(texPath, texExt.c_str());
			}
		);

		
	} 
	else
	{
		std::string texPath = defaultTexturePath;
		std::string texExt = std::filesystem::path(texPath).extension().string();
		std::string texKey = "tex:" + texPath;
		pTexture = graphics.m_textureCache.GetOrLoad(texKey, [&]() -> Texture*
			{
				return graphics.CreateTexture(texPath, texExt.c_str());
			}
		);
	}

	m_ownsMaterial = false;

	Texture* pNormal = nullptr;
	
	if (!firstSubMesh.material.normalTexturePath.empty())
	{ 
		std::string texPath = firstSubMesh.material.normalTexturePath;
		std::string texExt = std::filesystem::path(texPath).extension().string();
		std::string texKey = "tex:" + texPath;
		pNormal = graphics.m_textureCache.GetOrLoad(texKey, [&]() -> Texture* 
			{ 
				return graphics.CreateTexture(texPath, texExt.c_str()); 
			}
		);
	} 

	Texture* pSpecular = nullptr;
	
	if (!firstSubMesh.material.specularTexturePath.empty())
	{ 
		std::string texPath = firstSubMesh.material.specularTexturePath;
		std::string texExt = std::filesystem::path(texPath).extension().string();
		std::string texKey = "tex:" + texPath;
		pSpecular = graphics.m_textureCache.GetOrLoad(texKey, [&]() -> Texture* 
			{ 
				return graphics.CreateTexture(texPath, texExt.c_str()); 
			}
		);
	} 
	
	std::string meshKey = geomKey + "_mesh";
	m_pMesh = graphics.m_meshCache.GetOrLoad(meshKey, [&]() -> Mesh* 
		{ 
			return graphics.CreateMeshDeferred(m_pGeometry); 
		}
	); 
	m_ownsMesh = false;
	
	m_pMaterial = graphics.CreateMaterial();
	m_pMaterial->SetMaterialData(MaterialData::FromObjMaterial(firstSubMesh.material));
	
	if (pTexture) 
		m_pMaterial->SetTexture(pTexture, m_ownsMaterial);
	
	if (pNormal) m_pMaterial->SetNormalTexture(pNormal);
	
	if (pSpecular) m_pMaterial->SetSpecularTexture(pSpecular);
	
	m_primitive = true;
	
	for (size_t i = 1; i < pObjModel->subMeshes.size(); ++i)
	{ 
		ObjSubMesh& subMesh = pObjModel->subMeshes[i];
		GameObject* pChildObj = NEW GameObject(pParentObj->GetScene());
		pParentObj->AddChild(pChildObj);
		MeshRenderer& childMR = pChildObj->AddComponent<MeshRenderer>();
		
		childMR.m_pGeometry = subMesh.geometry;
		childMR.m_ownsGeometry = false;

		std::string childTexPath = subMesh.material.diffuseTexturePath;
		//std::string childTexPath = defaultTexturePath;
		Texture* childTexture = nullptr;
		Texture* pNormal = nullptr;
		Texture* pSpecular = nullptr;
		
		if (childTexPath.empty() && defaultTexturePath)
		{ 
			std::string texExt = std::filesystem::path(defaultTexturePath).extension().string();
			std::string texKey = "tex:" + std::string(defaultTexturePath);
			std::string texPath = std::string(defaultTexturePath);
			childTexture = graphics.m_textureCache.GetOrLoad(texKey, [&]() -> Texture* 
				{ 
					return graphics.CreateTexture(texPath, texExt.c_str()); 
				}
			); 

			childMR.m_ownsMaterial = false;
		}
		else
		{
			std::string texExt = std::filesystem::path(childTexPath).extension().string();
			std::string texKey = "tex:" + std::string(childTexPath);
			std::string texPath = std::string(childTexPath);
			childTexture = graphics.m_textureCache.GetOrLoad(texKey, [&]() -> Texture*
				{
					return graphics.CreateTexture(texPath, texExt.c_str());
				}
			);

			childMR.m_ownsMaterial = false;
					

			if (!subMesh.material.normalTexturePath.empty())
			{
				std::string texPath = subMesh.material.normalTexturePath;
				std::string texExt = std::filesystem::path(texPath).extension().string();
				std::string texKey = "tex:" + texPath;
				pNormal = graphics.m_textureCache.GetOrLoad(texKey, [&]() -> Texture*
					{
						return graphics.CreateTexture(texPath, texExt.c_str());
					}
				);
			}

			if (!subMesh.material.specularTexturePath.empty())
			{
				std::string texPath = subMesh.material.specularTexturePath;
				std::string texExt = std::filesystem::path(texPath).extension().string();
				std::string texKey = "tex:" + texPath;
				pSpecular = graphics.m_textureCache.GetOrLoad(texKey, [&]() -> Texture*
					{
						return graphics.CreateTexture(texPath, texExt.c_str());
					}
				);
			}
		}
		
		std::string childMeshKey = geomKey + "_mesh" + std::to_string(i);
		childMR.m_pMesh = graphics.m_meshCache.GetOrLoad(childMeshKey, [&]() -> Mesh* 
			{ 
				return graphics.CreateMeshDeferred(childMR.m_pGeometry); 
			}
		); 
		
		childMR.m_ownsMesh = false;
		
		childMR.m_pMaterial = graphics.CreateMaterial();

		if (childTexture) 
			childMR.m_pMaterial->SetTexture(childTexture, childMR.m_ownsMaterial);

		if (pNormal)
			childMR.m_pMaterial->SetNormalTexture(pNormal);

		if (pSpecular)
			childMR.m_pMaterial->SetSpecularTexture(pSpecular);

		childMR.m_pMaterial->SetMaterialData(MaterialData::FromObjMaterial(subMesh.material));

		childMR.m_primitive = true;
		pChildObj->SetName(("Child_" + std::to_string(i)).c_str()); 
	} 
}

void MeshRenderer::SetColor(Color c)
{
	GraphicEngine& graphics = *GameManager::GetWindow().GetGraphicEngine();
	graphics.SetColor(m_pGeometry, c);
	m_pMesh->UpdateMesh();
}

void MeshRenderer::SetTexture(const char* texturePath)
{
	GraphicEngine& graphics = *GameManager::GetWindow().GetGraphicEngine();

	std::string textureExtension = std::filesystem::path(texturePath).extension().string();
	std::string texPath = std::string(texturePath);
	Texture* pTexture = graphics.CreateTexture(texPath, textureExtension.c_str());

	m_pMaterial->SetTexture(pTexture);

	m_pMesh->UpdateMesh();
}

Geometry* MeshRenderer::GetGeometry()
{
	return m_pGeometry;
}

Mesh* MeshRenderer::GetMesh()
{
	return m_pMesh;
}

Material* MeshRenderer::GetMaterial()
{
	return m_pMaterial;
}

void MeshRenderer::SetSphere(Color c)
{
	SetSphere("DefaultTex.png", c);
}

void MeshRenderer::SetSphere(const char* texturePath, Color c)
{
	Free();

	GraphicEngine& graphics = *GameManager::GetWindow().GetGraphicEngine();


	std::string geomKey = "primitive:sphere";
	m_pGeometry = graphics.m_geometryCache.GetOrLoad(geomKey, [&]()->Geometry*
		{
			return graphics.CreatePrimitiveGeometry(SPHERE, c);
		});
	m_ownsGeometry = false;

	std::string textureExtension = std::filesystem::path(texturePath).extension().string();
	std::string texPath = std::string(texturePath);
	std::string texKey = std::string("tex:") + texturePath;
	Texture* pTexture = graphics.m_textureCache.GetOrLoad(texKey, [&]()->Texture*
		{
			return graphics.CreateTexture(texPath, textureExtension.c_str());
		});
	m_ownsMaterial = false;

	std::string meshKey = geomKey + std::string("_mesh");
	m_pMesh = graphics.m_meshCache.GetOrLoad(meshKey, [&]()->Mesh*
		{
			return graphics.CreateMeshDeferred(m_pGeometry);
		});
	m_ownsMesh = false;

	m_pMaterial = graphics.CreateMaterial();
	m_pMaterial->SetTexture(pTexture, m_ownsMaterial);

	m_primitive = true;
}

void MeshRenderer::Free()
{
	if (m_primitive == false) return;

	if (m_pGeometry && m_ownsGeometry)
	{
		delete m_pGeometry;
	}

	if (m_pMaterial)
	{
		delete m_pMaterial;
	}

	if (m_pMesh && m_ownsMesh)
	{
		delete m_pMesh;
	}

	m_pGeometry = nullptr;
	m_pMaterial = nullptr;
	m_pMesh = nullptr;

	m_primitive = false;
}
