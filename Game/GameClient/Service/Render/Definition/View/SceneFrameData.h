#pragma once
#include "GameClient/Graphics/RenderData/DirectionalLightData.h"
#include "SceneViewData.h"
#include "DrawShadowCasterItem.h"

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
