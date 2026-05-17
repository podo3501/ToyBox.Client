#pragma once
#include "GameClient/Service/Asset/Assets/MeshAsset.h"

class MeshFactory
{
public:
    static std::shared_ptr<MeshAsset> CreateTorus(
        float radius = 1.0f, //전체 반지름
        float tubeRadius = 0.3f, //도넛 두께
        uint32_t radialSegments = 32,
        uint32_t tubularSegments = 16);

    static std::shared_ptr<MeshAsset> CreateGrid(
        float cellSize = 1.0f,
        uint32_t halfExtent = 10);
};
