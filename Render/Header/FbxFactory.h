#ifndef FBXFACTORY_INCLUDED__H
#define FBXFACTORY_INCLUDED__H

#include <Tools/Header/PrimitiveTypes.h>
#include <vector>
#include <string>
#include <unordered_map>

struct VertexWeight
{
	uint32 jointIndex;
	float32 weight;
};

struct Mesh
{
	std::string name;
	std::vector<Vector3> positions;
	std::vector<Vector3> normals;
	std::vector<Vector3> tangentes;
	std::vector<Vector3> uvs0;
	std::vector<uint32> indices;

	std::vector<std::vector<VertexWeight>> boneWeights;

	int32 materialIndex = -1;
};

struct Material
{
	std::string name;
	Vector3 diffuseColor = { 1.f, 1.f, 1.f };
	Vector3 specularColor = { 1.f, 1.f, 1.f };
	Vector3 ambientColor = {0.f, 0.f, 0.f};
	float32 shininess = 0;

	std::unordered_map<std::string, std::string> textures;
};

struct Node
{
	std::string name;
	Matrix4x4 transform;
	int32 parent = -1;
	std::vector<int32> children;
};

struct Bone
{
	std::string name;
	int32 nodeIndex = -1;
	Matrix4x4 offsetMatrix;
};

struct KeyFrameVec3
{
	float64 time;
	Vector3 value;
};

struct KeyFrameQuat
{
	float64 time;
	float32 x, y, z, w;
};

struct AnimationChannel
{
	std::string nodeName;
	std::vector<KeyFrameVec3> positionKeys;
	std::vector<KeyFrameQuat> rotationKeys;
	std::vector<KeyFrameVec3> scalingKeys;
};

struct Animation
{
	std::string name;
	float64 duration = 0.0;
	float64 ticksPerSecond = 25.0;
	std::vector<AnimationChannel> channels;
};

struct CameraData
{
	std::string name;
	Vector3 position;
	Vector3 lookAt;
	float32 fov;
};

struct LightingData
{
	std::string name;
	Vector3 color;
	float32 intensity;
};

struct SceneData
	{
	std::vector<Mesh> meshes;
	std::vector<Material> materials;
	std::vector<Node> nodes;
	std::vector<Bone> bones;
	std::vector<Animation> animations;
	std::vector<CameraData> cameras;
	std::vector<LightingData> lights;
};


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postProcess.h>

class FbxFactory
{
public:
	static SceneData LoadFbxFile(const char* filePath);


private:
	static void EnsureVertexBoneWeights(Mesh& mesh);
	static int32 BuildNodeRecursive(const aiNode* pAiNode, int32 parentIndex, SceneData& out, std::unordered_map<const aiNode*, int32>& mapPtrToIndex);
	static void ProcessMeshes(const aiScene* pAiScene, SceneData& out, const std::unordered_map<const aiNode*, int32>& nodeMap);
	static void ProcessMaterials(const aiScene* pAiScene, SceneData& out);
	static void ProcessBonesAndWeights(const aiScene* pAiScene, SceneData& out, const std::unordered_map<const aiNode*, int32>& nodeMap);
	static void ProcessAnimations(const aiScene* pAiScene, SceneData& out);
	static void ProcessCamerasAndLights(const aiScene* pAiScene, SceneData& out);
	static void PrintSummary(const SceneData& scene);
	static Matrix4x4 AiMatrixToMatrix4x4(const aiMatrix4x4& aiMat);
};





#endif // !FBXFACTORY_INCLUDED__H