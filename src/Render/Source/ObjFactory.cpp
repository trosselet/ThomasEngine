#include "pch.h"
#include "Header/ObjFactory.h"

#include "Header/Geometry.h"
#include "Tools/Header/Color.h"
#include <fstream>

Geometry* ObjFactory::LoadObjFile(const char* filePath, Color color)
{
    Geometry* pGeometry = new Geometry();

    std::string path = "../../res/Gameplay/obj/" + std::string(filePath);

    std::ifstream file(path);
    std::string line;

    std::vector<DirectX::XMFLOAT3> positions;
    std::vector<DirectX::XMFLOAT2> uvs;
    std::vector<DirectX::XMFLOAT3> normals;

    int indexCount = 0;

    while (std::getline(file, line))
    {
        if (line.empty()) continue;

        if (line[0] == 'v')
        {
            if (line.length() > 1 && line[1] == ' ')
            {
                std::string posStr = line.substr(2);
                auto components = SplitString(posStr, ' ');

                if (components.size() >= 3)
                {
                    float x = std::stof(components[0]);
                    float y = std::stof(components[1]);
                    float z = std::stof(components[2]);
                    positions.emplace_back(x, y, z);
                }
            }
            else if (line.length() > 1 && line[1] == 't')
            {
                std::string uvStr = line.substr(3);
                auto components = SplitString(uvStr, ' ');

                if (components.size() >= 2)
                {
                    float u = std::stof(components[0]);
                    float v = std::stof(components[1]);
                    uvs.emplace_back(u, 1.0f - v);
                }
            }
            else if (line.length() > 1 && line[1] == 'n')
            {
                std::string normStr = line.substr(3);
                auto components = SplitString(normStr, ' ');

                if (components.size() >= 3)
                {
                    float nx = std::stof(components[0]);
                    float ny = std::stof(components[1]);
                    float nz = std::stof(components[2]);
                    normals.emplace_back(nx, ny, nz);
                }
            }
        }
        else if (line[0] == 'f')
        {
            std::string faceStr = line.substr(2);
            auto faceTokens = SplitString(faceStr, ' ');

            for (const auto& token : faceTokens)
            {
                auto indices = SplitString(token, '/');

                int posIdx = indices.size() > 0 && !indices[0].empty() ? std::stoi(indices[0]) - 1 : -1;
                int uvIdx = indices.size() > 1 && !indices[1].empty() ? std::stoi(indices[1]) - 1 : -1;
                int normIdx = indices.size() > 2 && !indices[2].empty() ? std::stoi(indices[2]) - 1 : -1;

                if (posIdx >= 0 && posIdx < positions.size())
                    pGeometry->positions.push_back(positions[posIdx]);

                if (uvIdx >= 0 && uvIdx < uvs.size())
                    pGeometry->UVs.push_back(uvs[uvIdx]);

                if (normIdx >= 0 && normIdx < normals.size())
                    pGeometry->normals.push_back(normals[normIdx]);

                pGeometry->colors.push_back({ color.r, color.g, color.b, color.a });
                pGeometry->indicies.push_back(indexCount++);
            }
        }
    }

    pGeometry->indexNumber = indexCount;
    return pGeometry;
}

std::vector<std::string> ObjFactory::SplitString(const std::string& input, char separator)
{
    std::vector<std::string> result;
    std::istringstream stream(input);
    std::string token;

    while (std::getline(stream, token, separator))
    {
        if (!token.empty())
            result.push_back(token);
    }

    return result;
}
