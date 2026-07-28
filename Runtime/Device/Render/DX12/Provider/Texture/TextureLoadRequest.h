#pragma once
#include <memory>
#include "GameClient/Asset/TextureAsset.h"

class TextureResource;

struct TextureLoadRequest
{
    std::shared_ptr<TextureResource> resource;
    std::shared_ptr<TextureAsset> asset;

    size_t estimatedBytes{ 0 };
};
