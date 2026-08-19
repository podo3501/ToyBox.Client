#pragma once
#include "GameClient/Graphics/RenderData/FrameData.h"
#include "GameClient/Service/Render/Resource/IResource.h"
#include "GameClient/Service/Render/Definition/View/SceneViewData.h"
#include "GameClient/Service/Render/Definition/Text/TextStyle.h"
#include "Core/Foundation/Geometry2D.h"
#include "Core/Foundation/Color.h"
#include "Core/Math/Vector2.h"

struct IRenderFrame
{
	virtual ~IRenderFrame() = default;
	virtual void SetFrameData(const FrameData& frameData) noexcept = 0;
	virtual void SubmitViews(std::vector<SceneViewData> views) = 0;
};
