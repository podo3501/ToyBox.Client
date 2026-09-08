#pragma once
#include "GameClient/Graphics/RenderData/DirectionalLightData.h"
#include "DrawShadowCasterItem.h"
#include "Scene/SceneViewData.h"
#include "Overlay/OverlayViewData.h"

struct SceneFrameData
{
	DirectionalLightData light;
	std::vector<DrawShadowCasterItem> shadowCasters;
	std::vector<SceneViewData> sceneViews;
	std::vector<OverlayViewData> overlayViews;

	void Clear()
	{
		light = {};
		shadowCasters.clear();
		sceneViews.clear();
		overlayViews.clear();
	}
};
