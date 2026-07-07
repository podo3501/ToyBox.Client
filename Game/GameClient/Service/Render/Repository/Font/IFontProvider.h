#pragma once
//#include "GameClient/Asset/FontAsset.h"
#include "GameClient/Service/Render/Resource/IFontResource.h"

struct IFontProvider
{
    virtual ~IFontProvider() = default;
    virtual shared_ptr<IFontResource> CreateResource() = 0;
};