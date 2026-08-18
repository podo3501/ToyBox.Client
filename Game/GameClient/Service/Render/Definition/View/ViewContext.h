#pragma once
#include "RenderState.h"
#include <optional>

struct Viewport
{
    float x{ 0.f };
    float y{ 0.f };
    float width{ 0.f };
    float height{ 0.f };
    float minDepth{ 0.f };
    float maxDepth{ 1.f };
};

struct RenderOverride
{
    std::optional<RasterPreset> rasterPreset;
};

struct ViewContext
{
    std::optional<::Viewport> viewport;
    RenderOverride renderOverride;
    // 나중에: Camera, RenderTargetSet, Viewport, PassType ...
};