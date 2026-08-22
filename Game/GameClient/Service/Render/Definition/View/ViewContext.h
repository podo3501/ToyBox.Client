#pragma once
#include "Core/Foundation/Geometry2D.h"
#include "RenderState.h"
#include "GameClient/Graphics/RenderData/CameraData.h"
#include <optional>

struct Viewport
{
    float x{ 0.f };
    float y{ 0.f };
    float width{ 0.f };
    float height{ 0.f };
    float minDepth{ 0.f };
    float maxDepth{ 1.f };
};

struct RenderOverride
{
    std::optional<RasterPreset> rasterPreset;
};

struct ViewContext
{
    CameraData camera;
    CameraData uiCamera; //기본적으로 정사영. 캐릭터 위에 체력바 같은걸 띄울려면 여기에 camera 를 셋팅.
    std::optional<Rect> viewport{ std::nullopt };
    RenderOverride renderOverride;
    // 나중에: Camera, RenderTargetSet, PassType ...
};