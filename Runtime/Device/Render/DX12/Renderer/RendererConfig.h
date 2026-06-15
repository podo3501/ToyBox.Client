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

//renderer를 만들때 숫자로 쓰이는 부분을 config으로 올린다.
//각 config은 각 폴더에 넣고 include 하자.
//각 config에 맞는 여러가지 것들을 넣어놓자.(shadow면 사이즈 등등)