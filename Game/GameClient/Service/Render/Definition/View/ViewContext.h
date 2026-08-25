#pragma once
#include "ViewIdentity.h"
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
    explicit ViewContext(ViewIdentity identity) : identity{ identity } {}

    ViewIdentity identity; //0은 invaild.
    CameraData camera;
    Core::Matrix uiProj; //기본적으로 정사영.
    std::optional<Rect> viewport{ std::nullopt };
    RenderOverride renderOverride;
    // 나중에: Camera, RenderTargetSet, PassType ...
};