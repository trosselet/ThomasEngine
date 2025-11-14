#include <Render/pch.h>
#include <Render/Header/ObjFactory.h>
#include <Render/Header/Geometry.h>
#include <Tools/Header/Color.h>

#include <fstream>
#include <string_view>
#include <cstdlib>

#if _EXE

#define OBJ_PATH "obj/"

#else

#define OBJ_PATH "obj/"

#endif

Geometry* ObjFactory::LoadObjFile(const char* filePath, Color color)
{
    Geometry* pGeometry = new Geometry();

    std::string path = OBJ_PATH + std::string(filePath);

    std::ifstream file(path, std::ios::binary);
    if (!file.is_open())
        return nullptr;

    ////////////////////////////////////////
    // READ ALL CHARACTER OF FILE IN CONTENT
    ////////////////////////////////////////
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    std::vector<DirectX::XMFLOAT3> positions;
    std::vector<DirectX::XMFLOAT2> uvs;
    std::vector<DirectX::XMFLOAT3> normals;

    positions.reserve(50000);
    uvs.reserve(50000);
    normals.reserve(50000);
    pGeometry->positions.reserve(70000);
    pGeometry->UVs.reserve(70000);
    pGeometry->normals.reserve(70000);
    pGeometry->indicies.reserve(70000);
    pGeometry->colors.reserve(70000);

    size_t indexCount = 0;


    ////////////////////////////////////////////
    // LAMBDA FUNCTION TO SPLIT STRING IN TOKENS
    ////////////////////////////////////////////
    auto split_fast = [](std::string_view s, char sep, std::vector<std::string_view>& out)
        {
            out.clear();
            size_t start = 0;
            for (size_t i = 0; i <= s.size(); ++i)
            {
                if (i == s.size() || s[i] == sep)
                {
                    if (i > start)
                        out.emplace_back(s.data() + start, i - start);
                    start = i + 1;
                }
            }
        };

    /////////////////////////////////////////////////////
    // "f 1/1/1/1 2/2/2 3/3/3" TO "1/1/1" "2/2/2" "3/3/3"
    /////////////////////////////////////////////////////
    std::vector<std::string_view> tokens;

    /////////////////////////////////////////////////////////////////
    // "1/1/1" "2/2/2" "3/3/3" TO "1" "1" "1" "2" "2" "2" "3" "3" "3"
    /////////////////////////////////////////////////////////////////
    std::vector<std::string_view> subTokens;

    size_t pos = 0;


    ///////////////////
    // PARSE CONTENT //
    ///////////////////
    while (pos < content.size())
    {
        size_t eol = content.find_first_of("\r\n", pos);
        if (eol == std::string::npos) eol = content.size();

        std::string_view line(content.data() + pos, eol - pos);
        pos = eol + 1;
        while (pos < content.size() && (content[pos] == '\n' || content[pos] == '\r')) ++pos;

        if (line.empty()) continue;

        if (line[0] == 'v')
        {
            if (line.size() > 1 && line[1] == ' ')
            {
                std::string_view data = line.substr(2);
                split_fast(data, ' ', tokens);
                if (tokens.size() >= 3)
                {
                    float x = strtof(std::string(tokens[0]).c_str(), nullptr);
                    float y = strtof(std::string(tokens[1]).c_str(), nullptr);
                    float z = strtof(std::string(tokens[2]).c_str(), nullptr);
                    positions.emplace_back(x, y, z);
                }
            }
            else if (line.size() > 1 && line[1] == 't')
            {
                std::string_view data = line.substr(3);
                split_fast(data, ' ', tokens);
                if (tokens.size() >= 2)
                {
                    float u = strtof(std::string(tokens[0]).c_str(), nullptr);
                    float v = strtof(std::string(tokens[1]).c_str(), nullptr);
                    uvs.emplace_back(u, 1.0f - v);
                }
            }
            else if (line.size() > 1 && line[1] == 'n')
            {
                std::string_view data = line.substr(3);
                split_fast(data, ' ', tokens);
                if (tokens.size() >= 3)
                {
                    float nx = strtof(std::string(tokens[0]).c_str(), nullptr);
                    float ny = strtof(std::string(tokens[1]).c_str(), nullptr);
                    float nz = strtof(std::string(tokens[2]).c_str(), nullptr);
                    normals.emplace_back(nx, ny, nz);
                }
            }
        }
        else if (line[0] == 'f')
        {
            std::string_view data = line.substr(2);
            split_fast(data, ' ', tokens);

            for (auto token : tokens)
            {
                split_fast(token, '/', subTokens);

                int posIdx = subTokens.size() > 0 && !subTokens[0].empty() ? strtol(std::string(subTokens[0]).c_str(), nullptr, 10) - 1 : -1;
                int uvIdx = subTokens.size() > 1 && !subTokens[1].empty() ? strtol(std::string(subTokens[1]).c_str(), nullptr, 10) - 1 : -1;
                int normIdx = subTokens.size() > 2 && !subTokens[2].empty() ? strtol(std::string(subTokens[2]).c_str(), nullptr, 10) - 1 : -1;

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

    pGeometry->indexNumber = static_cast<uint32>(indexCount);
    return pGeometry;
}
