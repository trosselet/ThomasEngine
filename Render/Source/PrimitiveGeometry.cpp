#include <Render/pch.h>
#include <Render/Header/PrimitiveGeometry.h>
#include <Render/Header/Geometry.h>

PrimitiveGeometry* PrimitiveGeometry::Get()
{
	static PrimitiveGeometry instance;

	return &instance;
}

Geometry* PrimitiveGeometry::GetPrimitiveGeometry(PrimitiveGeometryType primitiveType)
{
	return &PrimitiveGeometry::Get()->m_geometries[primitiveType];
}

void PrimitiveGeometry::InitializeGeometry()
{
    PrimitiveGeometry* pInstance = PrimitiveGeometry::Get();

    Geometry square;
    Geometry circle;
    Geometry cube;
    Geometry sphere;

    square.positions =
    {
        {-0.5f,  0.5f, 0.0f},
        { 0.5f,  0.5f, 0.0f},
        {-0.5f, -0.5f, 0.0f},
        { 0.5f, -0.5f, 0.0f}
    };

    square.indicies =
    {
        0, 1, 2,
        2, 1, 3
    };

    square.UVs =
    {
        { 0.0f, 1.0f },
        { 1.0f, 1.0f },
        { 0.0f, 0.0f },
        { 1.0f, 0.0f }
    };

    square.normals =
    {
        { 0.0f, 0.0f, 1.0f },
        { 0.0f, 0.0f, 1.0f },
        { 0.0f, 0.0f, 1.0f },
        { 0.0f, 0.0f, 1.0f }
    };

    square.indexNumber = 6;
    pInstance->m_geometries.push_back(square);


    uint32 detailLevel = 50;
    float32 radius = 0.5;

    circle.positions.push_back({ 0.0f, 0.0f, 0.0f });
    circle.UVs.push_back({ 0.5f, 0.5f });
    circle.normals.push_back({ 0.0f, 0.0f, 1.0f });


    float32 angleStep = DirectX::XM_2PI / detailLevel;
    for (uint32 i = 0; i < detailLevel; ++i)
    {
        float32 angle = i * angleStep;
        float32 x = radius * cosf(angle);
        float32 y = radius * sinf(angle);

        float32 u = 0.5f + 0.5f * cosf(angle);
        float32 v = 0.5f + 0.5f * sinf(angle);

        circle.positions.push_back({ x, y, 0.0f });
        circle.UVs.push_back({ u, v });
        circle.normals.push_back({ 0.0f, 0.0f, 1.0f });
    }

    for (uint32 i = 0; i < detailLevel; ++i)
    {
        uint32 current = i + 1;
        uint32 next = (i + 1) % detailLevel + 1;

        circle.indicies.push_back(0);
        circle.indicies.push_back(next);
        circle.indicies.push_back(current);
    }
    circle.indexNumber = (uint32)circle.indicies.size();
    pInstance->m_geometries.push_back(circle);

    cube.positions = {
        { -0.5f, -0.5f, -0.5f }, { 0.5f, -0.5f, -0.5f }, { 0.5f, 0.5f, -0.5f }, { -0.5f, 0.5f, -0.5f },     // Front face
        { 0.5f, -0.5f, -0.5f }, { 0.5f, -0.5f, 0.5f }, { 0.5f, 0.5f, 0.5f }, { 0.5f, 0.5f, -0.5f },         // Right face
        { 0.5f, -0.5f, 0.5f }, { -0.5f, -0.5f, 0.5f }, { -0.5f, 0.5f, 0.5f }, { 0.5f, 0.5f, 0.5f },         // Back face
        { -0.5f, -0.5f, 0.5f }, { -0.5f, -0.5f, -0.5f }, { -0.5f, 0.5f, -0.5f }, { -0.5f, 0.5f, 0.5f },     // Left face
        { -0.5f, 0.5f, -0.5f }, { 0.5f, 0.5f, -0.5f }, { 0.5f, 0.5f, 0.5f }, { -0.5f, 0.5f, 0.5f },         // Top face
        { -0.5f, -0.5f, 0.5f }, { 0.5f, -0.5f, 0.5f }, { 0.5f, -0.5f, -0.5f }, { -0.5f, -0.5f, -0.5f }      // Bottom face
    };

    cube.indicies = {
        2,  1,  0,
        3,  2,  0,

        6,  5,  4,
        7,  6,  4,

        10,  9,  8,
        11, 10,  8,

        14, 13, 12,
        15, 14, 12,

        18, 17, 16,
        19, 18, 16,

        22, 21, 20,
        23, 22, 20
    };

    cube.indexNumber = 36;

    cube.UVs = {
        { 0.0f, 1.0f }, { 1.0f, 1.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f },
        { 0.0f, 1.0f }, { 1.0f, 1.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f },
        { 0.0f, 1.0f }, { 1.0f, 1.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f },
        { 0.0f, 1.0f }, { 1.0f, 1.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f },
        { 0.0f, 1.0f }, { 1.0f, 1.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f },
        { 0.0f, 1.0f }, { 1.0f, 1.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f }
    };


    cube.normals = {
        { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f },         // Front face
        { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f },         // Right face
        { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f, -1.0f },     // Back face
        { -1.0f, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f },     // Left face
        { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f },         // Top face
        { 0.0f, -1.0f, 0.0f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, -1.0f, 0.0f }      // Bottom face
    };

    pInstance->m_geometries.push_back(cube);

    float phi = 0.f;
    float theta = 0.f;

    for (int i = 0; i <= detailLevel; ++i)
    {
        phi = DirectX::XM_PI * i / detailLevel;
        for (int j = 0; j <= detailLevel; ++j)
        {
            theta = 2 * DirectX::XM_PI * j / detailLevel;
            DirectX::XMFLOAT3 vertex;
            vertex.x = radius * sin(phi) * cos(theta);
            vertex.y = radius * cos(phi);
            vertex.z = radius * sin(phi) * sin(theta);
            sphere.positions.push_back(vertex);

            float u = 1.0f - (j / (float)detailLevel);
            float v = 1.0f - (i / (float)detailLevel);

            sphere.UVs.push_back(DirectX::XMFLOAT2(u, v));

            DirectX::XMFLOAT3 normal = { vertex.x, vertex.y, vertex.z };
            float length = sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
            normal.x /= length;
            normal.y /= length;
            normal.z /= length;
            sphere.normals.push_back(normal);
        }
    }

    for (int i = 0; i < detailLevel; ++i)
    {
        for (int j = 0; j < detailLevel; ++j)
        {
            int index = i * (detailLevel + 1) + j;
            sphere.indicies.push_back(index);
            sphere.indicies.push_back(index + 1);
            sphere.indicies.push_back(index + detailLevel + 1);

            sphere.indicies.push_back(index + 1);
            sphere.indicies.push_back(index + detailLevel + 2);
            sphere.indicies.push_back(index + detailLevel + 1);
        }
    }
    sphere.indexNumber = (uint32)sphere.indicies.size();
    pInstance->m_geometries.push_back(sphere);
}
