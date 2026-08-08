#pragma once
#include "GameClient/Asset/TextureAsset.h"
#include "GameClient/Service/Render/Resource/IBrushResource.h"
#include <memory>

struct IBrushProvider
{
    virtual ~IBrushProvider() = default;
    virtual std::shared_ptr<IBrushResource> CreateResource() = 0;
    virtual bool LoadResource(std::shared_ptr<IBrushResource> res, std::shared_ptr<TextureAsset> asset) = 0;
    virtual void ReleaseResource(std::shared_ptr<IBrushResource> res) = 0;
};
