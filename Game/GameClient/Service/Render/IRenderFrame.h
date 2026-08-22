#pragma once
#include "GameClient/Graphics/RenderData/FrameData.h"
#include "GameClient/Service/Render/Resource/IResource.h"
#include "GameClient/Service/Render/Definition/View/SceneFrameData.h"
#include "GameClient/Service/Render/Definition/Text/TextStyle.h"
#include "Core/Foundation/Geometry2D.h"
#include "Core/Foundation/Color.h"
#include "Core/Math/Vector2.h"

struct IRenderFrame
{
	virtual ~IRenderFrame() = default;
	virtual void SubmitFrame(SceneFrameData frame) noexcept = 0;
};
