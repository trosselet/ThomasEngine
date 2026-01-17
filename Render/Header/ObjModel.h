#ifndef OBJMODEL_INCLUDED__H
#define OBJMODEL_INCLUDED__H

#include <vector>
#include <string>

#include <Tools/Header/Color.h>

#include <Render/Header/Geometry.h>

class Texture;

struct ObjMaterial
{
	Color ambientColor;
	Color diffuseColor;
	Color specularColor;
	float shininess = 1.0f;
	float opacity = 1.0f;
	std::string diffuseTexturePath;
	std::string normalTexturePath;
	std::string specularTexturePath;
};

struct ObjSubMesh
{
    Geometry geometry;

	Texture* diffuseTexture = nullptr;
	Texture* normalTexture = nullptr;
	Texture* specularTexture = nullptr;

	ObjMaterial material;
    std::string materialName;
};

struct ObjModel
{
    std::vector<ObjSubMesh> subMeshes;
};

#endif
