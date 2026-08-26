#pragma once
#include "ViewID.h"
#include "Core/Foundation/Geometry2D.h"
#include "RenderState.h"
#include "GameClient/Graphics/RenderData/CameraData.h"
#include <optional>

struct RenderOverride
{
    std::optional<RasterPreset> rasterPreset;
};

struct ViewContext
{
    explicit ViewContext(ViewID id) : id{ id } {}

    ViewID id;
    CameraData camera;
    Core::Matrix uiProj; //기본적으로 정사영.
    std::optional<Rect> viewport{ std::nullopt };
    RenderOverride renderOverride;
    // 나중에: RenderTargetSet, PassType ...
};