#pragma once
#include <cstdint>
#include "GameClient/Service/Render/Definition/View/ViewID.h"

struct ShadowRendererConfig
{
    uint32_t maxObjectCount = 1024;
    uint32_t frameCBCount = 1; //shadow는 프레임당 뷰에 상관없이 한번만 돌기 때문에 1로 설정.
};

struct SurfaceRendererConfig
{
    uint32_t maxObjectCount = 1024;
    uint32_t maxViewCount{ static_cast<uint32_t>(ViewID::Count) };
};

struct DebugSurfaceRendererConfig
{
    uint32_t maxObjectCount = 1024;
};

struct UIRendererConfig
{
    uint32_t maxUI = 1024;
};

struct SkyboxRendererConfig
{
    uint32_t maxViewCount{ static_cast<uint32_t>(ViewID::Count) };
};

struct RendererConfig
{
    ShadowRendererConfig shadow;
    SurfaceRendererConfig surface;
    DebugSurfaceRendererConfig debug;
    UIRendererConfig ui;
    SkyboxRendererConfig skybox;
};