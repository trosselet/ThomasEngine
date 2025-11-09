#include "pch.h"
#include "Header/FbxFactory.h"
#include "Header/Geometry.h"

#include <fstream>
#include <string_view>
#include <cstdlib>

#if _EXE
#define FBX_PATH "../res/Gameplay/obj/"
#else
#define FBX_PATH "../../res/Gameplay/obj/"
#endif

Geometry* FbxFactory::LoadFbxFile(const char* filePath, Color color)
{
    Geometry* pGeometry = new Geometry();

    std::string fullPath = FBX_PATH + std::string(filePath);

    ///////////////
    // INIT FBX SDK
    ///////////////

    FbxManager* manager = FbxManager::Create();
    if (!manager)
    {
        Utils::DebugError("Error trying to initialize the fbx sdk");
        return nullptr;
    }

    FbxIOSettings* ios = FbxIOSettings::Create(manager, IOSROOT);
    manager->SetIOSettings(ios);

    FbxImporter* importer = FbxImporter::Create(manager, "Importer");
    if (!importer->Initialize(fullPath.c_str(), -1, manager->GetIOSettings()))
    {
        Utils::DebugError("Error importing the fbx: ", importer->GetStatus().GetErrorString());
        importer->Destroy();
        ios->Destroy();
        manager->Destroy();
        delete pGeometry;
        return nullptr;
    }

    FbxScene* scene = FbxScene::Create(manager, "scene");
    importer->Import(scene);

    FbxNode* rootNode = scene->GetRootNode();
    if (rootNode)
    {
        int meshCount = rootNode->GetChildCount();
        for (int i = 0; i < meshCount; ++i)
        {
            FbxNode* node = rootNode->GetChild(i);
            if (!node) continue;

            FbxMesh* fbxMesh = node->GetMesh();
            if (!fbxMesh) continue;

            /////////////////////////////
            // GET THE NAME OF THE UV SET
            /////////////////////////////

            FbxStringList uvNames;
            fbxMesh->GetUVSetNames(uvNames);
            const char* uvName = uvNames.GetCount() > 0 ? uvNames[0] : nullptr;

            int polyCount = fbxMesh->GetPolygonCount();
            for (int polyIdx = 0; polyIdx < polyCount; ++polyIdx)
            {
                int polySize = fbxMesh->GetPolygonSize(polyIdx);
                for (int vertIdx = 0; vertIdx < polySize; ++vertIdx)
                {
                    int ctrlPointIdx = fbxMesh->GetPolygonVertex(polyIdx, vertIdx);
                    if (ctrlPointIdx < 0) continue;

                    ////////////////////////////////////
                    // SET THE POSITION OF THE VERTICIES
                    ////////////////////////////////////

                    FbxVector4 pos = fbxMesh->GetControlPointAt(ctrlPointIdx);
                    pGeometry->positions.emplace_back(
                        static_cast<float>(pos[0]),
                        static_cast<float>(pos[1]),
                        static_cast<float>(pos[2])
                    );

                    ///////////////////////////////////////
                    // SET THE UV POSITION OF THE VERTICIES
                    ///////////////////////////////////////

                    if (uvName)
                    {
                        FbxVector2 uv;
                        bool unmapped;
                        fbxMesh->GetPolygonVertexUV(polyIdx, vertIdx, uvName, uv, unmapped);
                        //////////////
                        //INVERT THE V
                        //////////////
                        pGeometry->UVs.emplace_back(static_cast<float>(uv[0]), 1.0f - static_cast<float>(uv[1]));
                    }
                    else
                    {
                        pGeometry->UVs.emplace_back(0.0f, 0.0f);
                    }

                    //////////////////////////////////
                    // SET THE NORMAL OF THE VERTICIES
                    //////////////////////////////////

                    FbxVector4 n;
                    if (fbxMesh->GetPolygonVertexNormal(polyIdx, vertIdx, n))
                    {
                        pGeometry->normals.emplace_back(
                            static_cast<float>(n[0]),
                            static_cast<float>(n[1]),
                            static_cast<float>(n[2])
                        );
                    }
                    else
                    {
                        pGeometry->normals.emplace_back(0.0f, 0.0f, 0.0f);
                    }

                    /////////////////////////////////
                    // SET THE COLOR OF THE VERTICIES
                    /////////////////////////////////

                    pGeometry->colors.push_back({ color.r, color.g, color.b, color.a });

                    ////////////////////////////////////
                    // SET THE INDICIES OF THE VERTICIES
                    ////////////////////////////////////

                    pGeometry->indicies.push_back(static_cast<uint32>(pGeometry->positions.size() - 1));
                }
            }
        }
    }

    pGeometry->indexNumber = static_cast<uint32>(pGeometry->indicies.size());

    importer->Destroy();
    ios->Destroy();
    manager->Destroy();

    return pGeometry;
}
