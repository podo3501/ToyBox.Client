#pragma once
#include "../Base/ResourceRepository.h"
#include "Service/Render/Definition/EnvironmentDesc.h"
#include "Service/Render/Handle/EnvironmentHandle.h"
#include "Asset/EnvironmentAsset.h"

struct EnvironmentResourceTraits
{
    using Tag = EnvironmentTag;
    using Desc = EnvironmentDesc;
    using Asset = EnvironmentAsset;
};

using EnvironmentRepository = ResourceRepository<EnvironmentResourceTraits>;