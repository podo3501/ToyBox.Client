#pragma once
#include "ViewID.h"
#include "Core/Foundation/Geometry2D.h"
#include "GameClient/Service/Render/Definition/Shader/ShaderTypes.h"
#include "GameClient/Graphics/RenderData/CameraData.h"
#include <optional>

struct RenderOverride
{
    std::optional<RasterPreset> rasterPreset;
};

struct ViewContext
{
    explicit ViewContext(ViewID id) : id{ id } {}

    ViewID id{ InvalidViewID };
    CameraData camera;
    std::optional<Rect> viewport{ std::nullopt };
    RenderOverride renderOverride;
    // 나중에: RenderTargetSet, PassType ...
};