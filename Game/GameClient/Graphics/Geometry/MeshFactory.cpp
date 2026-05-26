#include "pch.h"
#include "MeshFactory.h"

std::shared_ptr<MeshAsset> MeshFactory::CreateTorus(
    float radius,
    float tubeRadius,
    uint32_t radialSegments,
    uint32_t tubularSegments)
{
    auto mesh = std::make_shared<MeshAsset>();
    mesh->format = VertexFormat::Mesh;

    std::vector<MeshVertex> vertices;
    std::vector<uint32_t> indices;

    vertices.reserve((radialSegments + 1) * (tubularSegments + 1));

    const float TWO_PI = 6.28318530718f;

    // 1. Vertex 생성
    for (uint32_t j = 0; j <= radialSegments; ++j)
    {
        float v = (float)j / (float)radialSegments;
        float phi = v * TWO_PI;

        float cosPhi = cosf(phi);
        float sinPhi = sinf(phi);

        for (uint32_t i = 0; i <= tubularSegments; ++i)
        {
            float u = (float)i / (float)tubularSegments;
            float theta = u * TWO_PI;

            float cosTheta = cosf(theta);
            float sinTheta = sinf(theta);

            // 중심 원 (major circle)
            float cx = radius * cosPhi;
            float cy = radius * sinPhi;

            // torus position
            MeshVertex vert;

            vert.px = (radius + tubeRadius * cosTheta) * cosPhi;
            vert.py = (radius + tubeRadius * cosTheta) * sinPhi;
            vert.pz = tubeRadius * sinTheta;

            // normal (center - vertex 방향)
            float nx = vert.px - cx;
            float ny = vert.py - cy;
            float nz = vert.pz;

            float len = sqrtf(nx * nx + ny * ny + nz * nz);
            if (len > 0.00001f)
            {
                nx /= len;
                ny /= len;
                nz /= len;
            }

            vert.nx = nx;
            vert.ny = ny;
            vert.nz = nz;

            vert.u = u;
            vert.v = v;

            vertices.push_back(vert);
        }
    }

    // 2. Index 생성
    const uint32_t ring = tubularSegments + 1;

    for (uint32_t j = 0; j < radialSegments; ++j)
    {
        for (uint32_t i = 0; i < tubularSegments; ++i)
        {
            uint32_t a = j * ring + i;
            uint32_t b = (j + 1) * ring + i;
            uint32_t c = (j + 1) * ring + (i + 1);
            uint32_t d = j * ring + (i + 1);

            // triangle 1
            indices.push_back(a);
            indices.push_back(b);
            indices.push_back(d);

            // triangle 2
            indices.push_back(b);
            indices.push_back(c);
            indices.push_back(d);
        }
    }

    // 3. MeshAsset 세팅
    mesh->SetVertices(vertices);
    mesh->indices = std::move(indices);

    return mesh;
}

std::shared_ptr<MeshAsset> MeshFactory::CreateGrid(
    float cellSize,
    uint32_t halfExtent,
    float r, float g, float b)
{
    auto mesh = std::make_shared<MeshAsset>();
    mesh->format = VertexFormat::Grid;

    std::vector<GridVertex> vertices;
    std::vector<uint32_t> indices;

    const uint32_t lineCount = (halfExtent * 2 + 1); // 라인 개수
    vertices.reserve(lineCount * 4); // 각 라인은 vertex 2개
    indices.reserve(lineCount * 4); // 각 라인은 index 2개

    uint32_t index = 0;

    const float min = -static_cast<float>(halfExtent) * cellSize;
    const float max = static_cast<float>(halfExtent) * cellSize;

    // Z 방향 라인 (세로줄)
    for (uint32_t i = 0; i < lineCount; ++i)
    {
        float x =
            min + static_cast<float>(i) * cellSize;

        GridVertex v0{};
        v0.px = x;
        v0.py = 0.0f;
        v0.pz = min;
        v0.r = r;
        v0.g = g;
        v0.b = b;

        GridVertex v1{};
        v1.px = x;
        v1.py = 0.0f;
        v1.pz = max;
        v1.r = r;
        v1.g = g;
        v1.b = b;

        vertices.push_back(v0);
        vertices.push_back(v1);

        indices.push_back(index++);
        indices.push_back(index++);
    }

    // X 방향 라인 (가로줄)
    for (uint32_t i = 0; i < lineCount; ++i)
    {
        float z = min + static_cast<float>(i) * cellSize;

        GridVertex v0{};
        v0.px = min;
        v0.py = 0.0f;
        v0.pz = z;
        v0.r = r;
        v0.g = g;
        v0.b = b;

        GridVertex v1{};
        v1.px = max;
        v1.py = 0.0f;
        v1.pz = z;
        v1.r = r;
        v1.g = g;
        v1.b = b;

        vertices.push_back(v0);
        vertices.push_back(v1);

        indices.push_back(index++);
        indices.push_back(index++);
    }

    mesh->SetVertices(vertices);
    mesh->indices = std::move(indices);

    return mesh;
}