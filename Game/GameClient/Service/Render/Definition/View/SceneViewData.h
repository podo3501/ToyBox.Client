#pragma once
#include "ViewContext.h"
#include "ViewDrawList.h"

struct SceneViewData
{
    ViewContext context{ InvalidViewID };
    ViewDrawList draws;
};