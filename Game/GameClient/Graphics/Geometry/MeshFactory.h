#pragma once
#include "GameClient/Asset/MeshAsset.h"

class MeshFactory
{
public:
    static std::shared_ptr<MeshAsset> CreateSphere(
        float radius = 0.5f,
        uint32_t sliceCount = 32,
        uint32_t stackCount = 32
    );

    static std::shared_ptr<MeshAsset> CreateCube(float size = 1.f);

    static std::shared_ptr<MeshAsset> CreateTorus(
        float radius = 1.0f, //전체 반지름
        float tubeRadius = 0.3f, //도넛 두께
        uint32_t radialSegments = 32,
        uint32_t tubularSegments = 16);

    static std::shared_ptr<MeshAsset> CreateGrid(
        float cellSize = 1.0f,
        uint32_t halfExtent = 10,
        float r = 0.5f, float g = 0.5f, float b = 0.5f);
};
