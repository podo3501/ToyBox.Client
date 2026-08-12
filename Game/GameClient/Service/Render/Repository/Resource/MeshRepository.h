#pragma once
#include "../Base/ResourceRepository.h"
#include "Service/Render/Handle/MeshHandle.h"
#include "Service/Render/Definition/MeshDesc.h"
#include "Asset/MeshAsset.h"

struct MeshResourceTraits
{
    using Tag = MeshTag;
    using Desc = MeshDesc;
    using Asset = MeshAsset;
};

using MeshRepository = ResourceRepository<MeshResourceTraits>;