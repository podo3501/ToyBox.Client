#pragma once
#include "GameCore/Service/Asset/Asset.h"

struct Vertex
{
    float px, py, pz;
    float nx, ny, nz;
    float u, v;
};

struct MeshAsset : public Asset
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};