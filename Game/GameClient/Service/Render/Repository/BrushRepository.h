#pragma once
#include "Base/ResourceRepository.h"
#include "IResourceProvider.h"
#include "Service/Render/Definition/BrushDesc.h"
#include "Service/Render/Handle/BrushHandle.h"
#include "Asset/TextureAsset.h"

struct BrushResourceTraits
{
    using Tag = BrushTag;
    using Desc = BrushDesc;
    using Asset = TextureAsset;
};

using BrushRepository = ResourceRepository<BrushResourceTraits>;
