#pragma once
#include <memory>
#include "GameClient/Asset/TextureCubeAsset.h"

class TextureCubeResource;

struct TextureCubeLoadRequest
{
    std::shared_ptr<TextureCubeResource> resource;
    std::shared_ptr<TextureCubeAsset> asset;

    size_t estimatedBytes{ 0 };
};
