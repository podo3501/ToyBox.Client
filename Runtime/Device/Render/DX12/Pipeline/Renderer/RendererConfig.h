#pragma once
#include <cstdint>

struct ShadowRendererConfig
{
    uint32_t maxObjectCount = 1024;
};

struct SurfaceRendererConfig
{
    uint32_t maxObjectCount = 1024;
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
    // 지금은 특별한 설정값 없음 - SurfaceRendererConfig(maxObjectCount 등)와 달리
    // 프레임당 1개만 그리므로 풀 확보가 필요 없음
};

struct RendererConfig
{
    ShadowRendererConfig shadow;
    SurfaceRendererConfig surface;
    DebugSurfaceRendererConfig debug;
    UIRendererConfig ui;
    SkyboxRendererConfig skybox;
};