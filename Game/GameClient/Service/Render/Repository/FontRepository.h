#pragma once
#include "Base/ResourceRepository.h"
#include "IResourceProvider.h"
#include "Service/Render/Definition/FontDesc.h"
#include "Service/Render/Handle/FontHandle.h"
#include "Asset/BinaryAsset.h"

struct FontResourceTraits
{
    using Tag = FontTag;
    using Desc = FontDesc;
    using Asset = BinaryAsset;
};

using FontRepository = ResourceRepository<FontResourceTraits>;