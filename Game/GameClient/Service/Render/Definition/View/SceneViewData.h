#pragma once
#include "ViewContext.h"
#include "ViewDrawList.h"

struct SceneViewData
{
    ViewContext context{ ViewID::Main };
    ViewDrawList draws;
};