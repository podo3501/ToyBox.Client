#pragma once
#include "../ViewTargetInfo.h"
#include "GameClient/Service/Render/Definition/Shader/ShaderTypes.h"

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