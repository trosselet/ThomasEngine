#include <Render/pch.h>
#include <Render/Header/FbxFactory.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postProcess.h>

using namespace FbxParser;

SceneData& FbxFactory::LoadFbxFile(const char* filePath)
{
	SceneData out = {};
	Assimp::Importer importer;
	uint32 importFlags =
		aiProcess_Triangulate |
		aiProcess_GenSmoothNormals |
		aiProcess_CalcTangentSpace |
		aiProcess_JoinIdenticalVertices |
		aiProcess_LimitBoneWeights |
		aiProcess_ImproveCacheLocality |
		aiProcess_ValidateDataStructure;

	std::string fullPath = std::string("assets/") + filePath;

	const aiScene* pAiScene = importer.ReadFile(fullPath, importFlags);

	if (!pAiScene)
	{
		Utils::DebugError("Failed to load FBX file: ", importer.GetErrorString());
		assert(false);
	}

	std::unordered_map<const aiNode*, int32> nodeMap;
	out.nodes.clear();
	BuildNodeRecursive(pAiScene->mRootNode, -1, out, nodeMap);

	ProcessMaterials(pAiScene, out);
	ProcessMeshes(pAiScene, out, nodeMap);
	ProcessBonesAndWeights(pAiScene, out, nodeMap);
	ProcessAnimations(pAiScene, out);
	ProcessCamerasAndLights(pAiScene, out);

	PrintSummary(out);

	return out;
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
	out.materials.reserve(pAiScene->mNumMaterials);

	for (uint32 i = 0; i < pAiScene->mNumMaterials; ++i)
	{
		aiMaterial* am = pAiScene->mMaterials[i];
		Material material;
		aiString name;
		if (AI_SUCCESS == am->Get(AI_MATKEY_NAME, name))
		{
			material.name = name.C_Str();
		}
		aiColor3D color;
		if (AI_SUCCESS == am->Get(AI_MATKEY_COLOR_DIFFUSE, color))
		{
			material.diffuseColor = Vector3(color.r, color.g, color.b);
		}
		if (AI_SUCCESS == am->Get(AI_MATKEY_COLOR_SPECULAR, color))
		{
			material.specularColor = Vector3(color.r, color.g, color.b);
		}
		if (AI_SUCCESS == am->Get(AI_MATKEY_COLOR_AMBIENT, color))
		{
			material.ambientColor = Vector3(color.r, color.g, color.b);
		}
		float shininess;
		if (AI_SUCCESS == am->Get(AI_MATKEY_SHININESS, shininess))
		{
			material.shininess = shininess;
		}

		aiTextureType textureTypes[] = {
			aiTextureType_DIFFUSE,
			aiTextureType_SPECULAR,
			aiTextureType_NORMALS,
			aiTextureType_HEIGHT,
			aiTextureType_EMISSIVE
		};

		const char* texKeys[] = 
		{
			"diffuse",
			"specular",
			"normal",
			"height",
			"emissive"
		};

		for (size_t t = 0; t < sizeof(textureTypes) / sizeof(textureTypes[0]); ++t)
		{
			aiTextureType texType = textureTypes[t];
			unsigned int texCount = am->GetTextureCount(texType);
			if (texCount > 0)
			{
				aiString texPath;
				if (AI_SUCCESS == am->GetTexture(texType, 0, &texPath))
				{
					material.textures[texKeys[t]] = texPath.C_Str();
				}
			}
		}

		out.materials.push_back(std::move(material));
	}

}

void FbxFactory::ProcessBonesAndWeights(const aiScene* pAiScene, SceneData& out, const std::unordered_map<const aiNode*, int32>& nodeMap)
{
	std::unordered_map<std::string, int32> boneNameToIndex;

	for (uint32 mi = 0; mi < pAiScene->mNumMeshes; ++mi)
	{
		aiMesh* am = pAiScene->mMeshes[mi];
		for (uint32 b = 0; b < am->mNumBones; ++b)
		{
			aiBone* ab = am->mBones[b];
			std::string bName = ab->mName.C_Str();
			if (boneNameToIndex.find(bName) == boneNameToIndex.end())
			{
				Bone bone;
				bone.name = bName;
				bone.offsetMatrix = AiMatrixToMatrix4x4(ab->mOffsetMatrix);
				int nodeIndex = -1;

				for (size_t ni = 0; ni < out.nodes.size(); ++ni)
				{
					if (out.nodes[ni].name == bName)
					{
						nodeIndex = static_cast<int>(ni);
						break;
					}
				}

				bone.nodeIndex = nodeIndex;
				int32 boneIndex = static_cast<int32>(out.bones.size());
				boneNameToIndex[bName] = boneIndex;
				out.bones.push_back(std::move(bone));
			}
		}		
	}

	for (uint32 mi = 0; mi < pAiScene->mNumMeshes; ++mi)
	{
		aiMesh* am = pAiScene->mMeshes[mi];
		Mesh& mesh = out.meshes[mi];
		EnsureVertexBoneWeights(mesh);
		
		for (uint32 b = 0; b < am->mNumBones; ++b)
		{
			aiBone* ab = am->mBones[b];
			std::string bName = ab->mName.C_Str();
			int32 boneIndex = boneNameToIndex[bName];
			for (uint32 w = 0; w < ab->mNumWeights; ++w)
			{
				aiVertexWeight& aw = ab->mWeights[w];
				uint32 vertexId = aw.mVertexId;
				float32 weight = aw.mWeight;
				mesh.boneWeights[vertexId].push_back({ static_cast<uint32>(boneIndex), weight });
			}
		}

	}

}

void FbxFactory::ProcessAnimations(const aiScene* pAiScene, SceneData& out)
{
	out.animations.reserve(pAiScene->mNumAnimations);

	for (uint32 a = 0; a < pAiScene->mNumAnimations; ++a)
	{
		aiAnimation* anim = pAiScene->mAnimations[a];
		Animation animation;
		animation.name = anim->mName.C_Str();
		animation.duration = anim->mDuration;
		animation.ticksPerSecond = anim->mTicksPerSecond != 0.0 ? anim->mTicksPerSecond : 25.0;

		for (uint32 c = 0; c < anim->mNumChannels; ++c)
		{
			aiNodeAnim* chan = anim->mChannels[c];
			AnimationChannel channel;
			channel.nodeName = chan->mNodeName.C_Str();

			for (uint32 pk = 0; pk < chan->mNumPositionKeys; ++pk)
			{
				aiVectorKey& pv = chan->mPositionKeys[pk];
				channel.positionKeys.push_back({ pv.mTime, Vector3(pv.mValue.x, pv.mValue.y, pv.mValue.z) });
			}
			for (uint32 rk = 0; rk < chan->mNumRotationKeys; ++rk)
			{
				aiQuatKey& rv = chan->mRotationKeys[rk];
				channel.rotationKeys.push_back({ rv.mTime, rv.mValue.w, rv.mValue.x, rv.mValue.y, rv.mValue.z });
			}
			for (uint32 sk = 0; sk < chan->mNumScalingKeys; ++sk)
			{
				aiVectorKey& sv = chan->mScalingKeys[sk];
				channel.scalingKeys.push_back({ sv.mTime, Vector3(sv.mValue.x, sv.mValue.y, sv.mValue.z) });
			}

			animation.channels.push_back(std::move(channel));
		}
		out.animations.push_back(std::move(animation));
	}
}

void FbxFactory::ProcessCamerasAndLights(const aiScene* pAiScene, SceneData& out)
{
	for (uint32 i = 0; i < pAiScene->mNumCameras; ++i)
	{
		aiCamera* ac = pAiScene->mCameras[i];
		CameraData camera;
		camera.name = ac->mName.C_Str();
		camera.position = Vector3(ac->mPosition.x, ac->mPosition.y, ac->mPosition.z);
		camera.lookAt = Vector3(ac->mLookAt.x, ac->mLookAt.y, ac->mLookAt.z);
		camera.fov = ac->mHorizontalFOV;
		out.cameras.push_back(std::move(camera));
	}

	for (uint32 i = 0; i < pAiScene->mNumLights; ++i)
	{
		aiLight* al = pAiScene->mLights[i];
		LightingData light;
		light.name = al->mName.C_Str();
		light.color = Vector3(al->mColorDiffuse.r, al->mColorDiffuse.g, al->mColorDiffuse.b);
		light.intensity = 1.0f;
		out.lights.push_back(std::move(light));
	}

}

void FbxFactory::PrintSummary(const SceneData& scene)
{
	Utils::DebugLog("FBX Import Summary:");
	Utils::DebugLog("  Meshes: ", scene.meshes.size());
	Utils::DebugLog("  Materials: ", scene.materials.size());
	Utils::DebugLog("  Nodes: ", scene.nodes.size());
	Utils::DebugLog("  Bones: ", scene.bones.size());
	Utils::DebugLog("  Animations: ", scene.animations.size());
	Utils::DebugLog("  Cameras: ", scene.cameras.size());
	Utils::DebugLog("  Lights: ", scene.lights.size());
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
