#pragma once
#include "../Base/ResourceRepository.h"
#include "Service/Render/Handle/DebugMaterialHandle.h"
#include "Service/Render/Definition/GridDebugMaterialDesc.h"
#include "Asset/AssetData.h"

struct DebugMaterialResourceTraits
{
    using Tag = DebugMaterialTag;
    using Desc = GridDebugMaterialDesc;
    using Asset = AssetData; // 현재 Debug Material은 Asset을 사용하지 않음.
};

using DebugMaterialRepository = ResourceRepository<DebugMaterialResourceTraits>;