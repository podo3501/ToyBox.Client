#pragma once
#include "GameCore/Service/Asset/Asset.h"

struct MeshAsset : public Asset
{
    struct Vertex
    {
        float px, py, pz;
        float nx, ny, nz;
        float u, v;
    };

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};