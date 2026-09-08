#pragma once
#include "ViewID.h"
#include "GameClient/Graphics/RenderData/CameraData.h"
#include "Core/Foundation/Geometry2D.h"
#include <optional>

struct ViewTargetInfo
{
    ViewID id{ InvalidViewID };
    CameraData camera;
    std::optional<Rect> viewport{ std::nullopt };
};
