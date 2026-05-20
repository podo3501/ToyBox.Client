#pragma once
#include <memory>
#include "GameClient/Service/Asset/Assets/TextureAsset.h"
#include "GameClient/Service/Render/Desc/TextureDesc.h"

struct ITextureResource;

struct TextureLoadRequest
{
    std::shared_ptr<ITextureResource> resource;
    std::shared_ptr<TextureAsset> asset;
    TextureDesc desc;

    size_t estimatedBytes{ 0 };
};
