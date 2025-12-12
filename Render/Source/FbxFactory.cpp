#include <Render/pch.h>
#include <Render/Header/FbxFactory.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postProcess.h>

SceneData FbxFactory::LoadFbxFile(const char* filePath)
{
	return SceneData();
}

void FbxFactory::EnsureVertexBoneWeights(Mesh& mesh)
{
	if (mesh.boneWeights.size() != mesh.positions.size())
	{
		mesh.boneWeights.assign(mesh.positions.size(), {});
	}
}

int32 FbxFactory::BuildNodeRecursive(const aiNode* pAiNode, int32 parentIndex, SceneData& out, std::unordered_map<const aiNode*, int32>& mapPtrToIndex)
{
	Node node;
	node.name = pAiNode->mName.C_Str();
	node.transform = AiMatrixToMatrix4x4(pAiNode->mTransformation);
	node.parent = parentIndex;
	int myIndex = static_cast<int32>(out.nodes.size());
	out.nodes.push_back(node);
	mapPtrToIndex[pAiNode] = myIndex;

	for (uint32 i = 0; i < pAiNode->mNumChildren; i++)
	{
		int32 childIndex = BuildNodeRecursive(pAiNode->mChildren[i], myIndex, out, mapPtrToIndex);
		out.nodes[myIndex].children.push_back(childIndex);
	}

	return myIndex;
}

void FbxFactory::ProcessMeshes(const aiScene* pAiScene, SceneData& out, const std::unordered_map<const aiNode*, int32>& nodeMap)
{
	out.meshes.reserve(pAiScene->mNumMeshes);
	
	for (uint32 m = 0; m < pAiScene->mNumMeshes; ++m)
	{
		aiMesh* am = pAiScene->mMeshes[m];
		Mesh mesh;
		mesh.name = am->mName.C_Str();
		mesh.materialIndex = static_cast<int32>(am->mMaterialIndex);

		// Process vertices
		mesh.positions.resize(am->mNumVertices);
		for (uint32 i = 0; i < am->mNumVertices; ++i)
		{
			mesh.positions[i] = Vector3(am->mVertices[i].x, am->mVertices[i].y, am->mVertices[i].z);
		}

		if (am->HasNormals())
		{
			mesh.normals.resize(am->mNumVertices);
			for (uint32 i = 0; i < am->mNumVertices; ++i)
			{
				mesh.normals[i] = Vector3(am->mNormals[i].x, am->mNormals[i].y, am->mNormals[i].z);
			}
		}
		if (am->HasTangentsAndBitangents())
		{
			mesh.tangentes.resize(am->mNumVertices);
			for (uint32 i = 0; i < am->mNumVertices; ++i)
			{
				mesh.tangentes[i] = Vector3(am->mTangents[i].x, am->mTangents[i].y, am->mTangents[i].z);
			}
		}
		if (am->HasTextureCoords(0))
		{
			mesh.uvs0.resize(am->mNumVertices);
			for (uint32 i = 0; i < am->mNumVertices; ++i)
			{
				mesh.uvs0[i] = Vector3(am->mTextureCoords[0][i].x, am->mTextureCoords[0][i].y, 0.0f);
			}
		}

		// Process indices
		for (uint32 f = 0; f < am->mNumFaces; ++f)
		{
			aiFace& face = am->mFaces[f];
			for (uint32 idx = 0; idx < face.mNumIndices; ++idx)
			{
				mesh.indices.push_back(face.mIndices[idx]);
			}
		}

		mesh.boneWeights.resize(mesh.positions.size());
		if (am->HasBones())
		{
			for (uint32 b = 0; b < am->mNumBones; ++b)
			{
				aiBone* ab = am->mBones[b];
			}
		}

		out.meshes.push_back(std::move(mesh));
	}
}

void FbxFactory::ProcessMaterials(const aiScene* pAiScene, SceneData& out)
{
}

void FbxFactory::ProcessBonesAndWeights(const aiScene* pAiScene, SceneData& out, const std::unordered_map<const aiNode*, int32>& nodeMap)
{
}

void FbxFactory::ProcessAnimations(const aiScene* pAiScene, SceneData& out)
{
}

void FbxFactory::ProcessCamerasAndLights(const aiScene* pAiScene, SceneData& out)
{
}

void FbxFactory::PrintSummary(const SceneData& scene)
{
}

Matrix4x4 FbxFactory::AiMatrixToMatrix4x4(const aiMatrix4x4& aiMat)
{
	Matrix4x4 mat;

	mat.m[0][0] = aiMat.a1; mat.m[0][1] = aiMat.b1; mat.m[0][2] = aiMat.c1; mat.m[0][3] = aiMat.d1;
	mat.m[1][0] = aiMat.a2; mat.m[1][1] = aiMat.b2; mat.m[1][2] = aiMat.c2; mat.m[1][3] = aiMat.d2;
	mat.m[2][0] = aiMat.a3; mat.m[2][1] = aiMat.b3; mat.m[2][2] = aiMat.c3; mat.m[2][3] = aiMat.d3;
	mat.m[3][0] = aiMat.a4; mat.m[3][1] = aiMat.b4; mat.m[3][2] = aiMat.c4; mat.m[3][3] = aiMat.d4;

	return mat;
}
