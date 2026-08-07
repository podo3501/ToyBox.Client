#pragma once
#include "GameClient/Asset/BinaryAsset.h"
#include "GameClient/Service/Render/Resource/IFontResource.h"

struct IFontProvider
{
    virtual ~IFontProvider() = default;
    virtual std::shared_ptr<IFontResource> CreateResource() = 0;
    virtual bool LoadResource(std::shared_ptr<IFontResource> resource, std::shared_ptr<BinaryAsset> asset) = 0;
    virtual void ReleaseResource(std::shared_ptr<IFontResource> resource) = 0;
};