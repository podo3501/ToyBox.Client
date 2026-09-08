#pragma once
#include "OverlayViewContext.h"
#include "OverlayViewDrawList.h"

struct OverlayViewData
{
    OverlayViewContext context{ InvalidViewID };
    OverlayViewDrawList draws;
};