#pragma once
#include "SceneViewContext.h"
#include "SceneViewDrawList.h"

struct SceneViewData
{
    SceneViewContext context{ InvalidViewID };
    SceneViewDrawList draws;
};