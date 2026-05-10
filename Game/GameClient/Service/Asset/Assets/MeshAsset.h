#pragma once
#include <vector>
#include "GameCore/Service/Asset/Asset.h"

enum class VertexFormat
{
    Mesh,
    UI,
};

struct MeshVertex
{
    float px, py, pz;
    float nx, ny, nz;
    float u, v;
};

struct UIVertex
{
    float x, y, z;
    float r, g, b, a;
    float u, v;
};

struct MeshAsset : public Asset
{
    VertexFormat format;

    uint32_t vertexStride{ 0 }; //generic하게 byte로 바뀌었기 때문에 보폭을 저장해야 한다.
    uint32_t vertexCount{ 0 };

    std::vector<std::byte> vertices;
    std::vector<uint32_t> indices;

    template<typename T>
    void SetVertices(const std::vector<T>& src)
    {
        vertexStride = sizeof(T);
        vertexCount = static_cast<uint32_t>(src.size());

        vertices.resize(sizeof(T) * src.size());
        memcpy(vertices.data(), src.data(), vertices.size());
    }
};