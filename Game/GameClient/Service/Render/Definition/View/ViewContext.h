#pragma once
#include "ViewID.h"
#include "Core/Foundation/Geometry2D.h"
#include "GameClient/Service/Render/Definition/Shader/ShaderTypes.h"
#include "GameClient/Graphics/RenderData/CameraData.h"
#include <optional>

struct ViewTargetInfo
{
    ViewID id{ InvalidViewID };
    CameraData camera;
    std::optional<Rect> viewport{ std::nullopt };
};

struct RenderOverride
{
    std::optional<RasterPreset> rasterPreset;
};

struct SceneViewContext
{
    explicit SceneViewContext(ViewID id) : target{ id } {}
    ViewTargetInfo target;
    RenderOverride renderOverride;
};

struct OverlayViewContext
{
    explicit OverlayViewContext(ViewID id) : target{ id } {}
    ViewTargetInfo target;
};