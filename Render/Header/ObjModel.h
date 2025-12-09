#ifndef OBJMODEL_INCLUDED__H
#define OBJMODEL_INCLUDED__H

#include <vector>
#include <string>

struct Geometry;
class Texture;

struct ObjSubMesh
{
    Geometry* geometry = nullptr;
    Texture* texture = nullptr;
    std::string materialName;
};

struct ObjModel
{
    std::vector<ObjSubMesh> subMeshes;
};

#endif
