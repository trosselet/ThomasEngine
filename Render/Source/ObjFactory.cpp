#include <Render/pch.h>
#include <Render/Header/ObjFactory.h>
#include <Render/Header/Geometry.h>
#include <Render/Header/ObjModel.h>
#include <Tools/Header/Color.h>

#include <filesystem>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

ObjModel* ObjFactory::LoadObjFile(const char* filePath, Color color)
{

    std::string fullPath = std::string("assets/") + filePath;

    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(
        fullPath,
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals |
        aiProcess_JoinIdenticalVertices |
        aiProcess_ImproveCacheLocality |
        aiProcess_PreTransformVertices |
        aiProcess_CalcTangentSpace
    );

    if (!scene || !scene->HasMeshes())
        return nullptr;

    ObjModel* model = new ObjModel();

    for (unsigned int m = 0; m < scene->mNumMeshes; ++m)
    {
        aiMesh* mesh = scene->mMeshes[m];
        ObjSubMesh subMesh;

        Geometry* geom = new Geometry();
        geom->positions.reserve(mesh->mNumVertices);
        geom->UVs.reserve(mesh->mNumVertices);
        geom->normals.reserve(mesh->mNumVertices);
        geom->colors.reserve(mesh->mNumVertices);
        geom->indicies.reserve(mesh->mNumFaces * 3);

        for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
        {
            aiVector3D pos = mesh->mVertices[i];
            geom->positions.emplace_back(pos.x, pos.y, pos.z);

            if (mesh->mTextureCoords[0])
            {
                aiVector3D uv = mesh->mTextureCoords[0][i];
                geom->UVs.emplace_back(uv.x, 1.f - uv.y);
            }
            else
            {
                geom->UVs.emplace_back(0.f, 0.f);
            }

            if (mesh->HasNormals())
            {
                aiVector3D norm = mesh->mNormals[i];
                geom->normals.emplace_back(norm.x, norm.y, norm.z);
            }
            else
            {
                geom->normals.emplace_back(0.f, 1.f, 0.f);
            }

            geom->colors.emplace_back(color.r, color.g, color.b, color.a);
        }

        for (unsigned int f = 0; f < mesh->mNumFaces; ++f)
        {
            const aiFace& face = mesh->mFaces[f];
            for (unsigned int i = 0; i < face.mNumIndices; ++i)
            {
                geom->indicies.push_back(face.mIndices[i]);
            }
        }

        geom->indexNumber = (uint32)geom->indicies.size();
        subMesh.geometry = geom;

        aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
        std::string extension;

		ObjMaterial material;

		aiColor3D diffuse(1.f, 1.f, 1.f);
        if (mat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse) == AI_SUCCESS)
        {
			material.diffuseColor = Color(diffuse.r, diffuse.g, diffuse.b, 1.f);
        }

		aiColor3D ambient(1.f, 1.f, 1.f);
        if (mat->Get(AI_MATKEY_COLOR_AMBIENT, ambient) == AI_SUCCESS)
		{
			material.ambientColor = Color(ambient.r, ambient.g, ambient.b, 1.f);
		}

		aiColor3D specular(1.f, 1.f, 1.f);
        if (mat->Get(AI_MATKEY_COLOR_SPECULAR, specular) == AI_SUCCESS)
        {
			material.specularColor = Color(specular.r, specular.g, specular.b, 1.f);
        }

		float shininess = 1.f;
        if (mat->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS)
		{
			material.shininess = shininess;
		}

		float opacity = 1.f;
		if (mat->Get(AI_MATKEY_OPACITY, opacity) == AI_SUCCESS)
		{
			material.opacity = opacity;
		}



        aiString texPath;
        if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0)
        {
            if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS)
            {
                std::string fullTexturePath = texPath.C_Str();

                extension = std::filesystem::path(fullTexturePath).extension().string();

				material.diffuseTexturePath = std::filesystem::path(fullTexturePath).filename().string();

            }
        }

        if (mat->GetTextureCount(aiTextureType_NORMALS) > 0)
        {
            if (mat->GetTexture(aiTextureType_NORMALS, 0, &texPath) == AI_SUCCESS)
            {
				std::string fullTexturePath = texPath.C_Str();
                material.normalTexturePath = std::filesystem::path(fullTexturePath).filename().string();
            }
		}

        if (mat->GetTextureCount(aiTextureType_SPECULAR) > 0)
        {
            if (mat->GetTexture(aiTextureType_SPECULAR, 0, &texPath) == AI_SUCCESS)
			{
                std::string fullTexturePath = texPath.C_Str();
                material.specularTexturePath = std::filesystem::path(fullTexturePath).filename().string();
			}
		}

		subMesh.material = material;

        subMesh.materialName = mat->GetName().C_Str() + extension;

        model->subMeshes.push_back(subMesh);
    }

    return model;
}