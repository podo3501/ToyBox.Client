#pragma once
#include "GameClient/Asset/TextureAsset.h"
#include "GameClient/Service/Render/Resource/IBrushResource.h"
#include <memory>

struct IBrushProvider
{
    virtual ~IBrushProvider() = default;
    virtual std::shared_ptr<IResource> CreateResource() = 0;
    virtual bool LoadResource(std::shared_ptr<IResource> res, std::shared_ptr<TextureAsset> asset) = 0;
    virtual void ReleaseResource(std::shared_ptr<IResource> res) = 0;
};
