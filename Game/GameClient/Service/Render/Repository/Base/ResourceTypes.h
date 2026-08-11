#pragma once
#include "Core/Utils/Hash.h"
#include "Core/Utils/EnumHelpers.h"

enum class LoadState
{
    Pending,
    AssetLoading,
    ResourceLoading,
    Ready,
    Failed
};