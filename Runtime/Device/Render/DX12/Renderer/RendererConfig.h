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

struct RendererConfig
{
    ShadowRendererConfig shadow;
    SurfaceRendererConfig surface;
    DebugSurfaceRendererConfig debug;
    UIRendererConfig ui;
};