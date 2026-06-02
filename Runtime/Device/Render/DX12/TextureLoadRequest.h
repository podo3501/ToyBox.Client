#pragma once
#include <memory>
#include "GameClient/Service/Asset/Assets/TextureAsset.h"
#include "GameClient/Service/Render/Desc/TextureDesc.h"

class TextureResource;

struct TextureLoadRequest
{
    std::shared_ptr<TextureResource> resource;
    std::shared_ptr<TextureAsset> asset;

    size_t estimatedBytes{ 0 };
};
