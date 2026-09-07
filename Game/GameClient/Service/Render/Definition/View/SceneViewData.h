#pragma once
#include "ViewContext.h"
#include "ViewDrawList.h"

struct SceneViewData
{
    SceneViewContext context{ InvalidViewID };
    SceneDrawList draws;
};

struct OverlayViewData
{
    OverlayViewContext context{ InvalidViewID };
    OverlayDrawList draws;
};