#pragma once
#include "../Base/ResourceRepository.h"
#include "Service/Render/Handle/DebugMeshHandle.h"
#include "Service/Render/Definition/DebugMeshDesc.h"
#include "Asset/MeshAsset.h"

struct DebugMeshResourceTraits
{
    using Tag = DebugMeshTag;
    using Desc = DebugMeshDesc;
    using Asset = MeshAsset;
};

using DebugMeshRepository = ResourceRepository<DebugMeshResourceTraits>;