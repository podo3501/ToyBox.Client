#pragma once
#include "ViewContext.h"
#include "ViewDrawList.h"

struct SceneView
{
    ViewContext context;
    ViewDrawList draws;
};