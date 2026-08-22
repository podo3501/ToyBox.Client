#pragma once
#include "ViewContext.h"
#include "ViewDrawList.h"

struct SceneViewData
{
    ViewContext context{ ViewIdentity::Invalid() };
    ViewDrawList draws;
};