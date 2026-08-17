#pragma once
#include "RenderState.h"
#include <optional>

struct RenderOverride
{
    std::optional<RasterPreset> rasterPreset;
};

struct ViewContext
{
    RenderOverride renderOverride;
    // 나중에: Camera, RenderTargetSet, Viewport, PassType ...
};