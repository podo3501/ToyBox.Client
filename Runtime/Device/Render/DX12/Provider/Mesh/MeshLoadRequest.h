#pragma once
#include <memory>
#include "GameClient/Asset/MeshAsset.h"

class MeshResource;

struct MeshLoadRequest
{
    std::shared_ptr<MeshResource> resource;
    std::shared_ptr<MeshAsset> asset;

    size_t vbBytes{ 0 };
    size_t ibBytes{ 0 };
    size_t estimatedBytes{ 0 };
};