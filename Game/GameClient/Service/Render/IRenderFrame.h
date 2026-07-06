#pragma once
#include "GameClient/Graphics/RenderData/FrameData.h"
#include "GameClient/Service/Render/Resource/IMeshResource.h"
#include "GameClient/Service/Render/Resource/IMaterialResource.h"
#include "Core/Foundation/Geometry2D.h"

struct IRenderFrame
{
	virtual ~IRenderFrame() = default;
	virtual void SetFrameData(const FrameData& frameData) noexcept = 0;

	virtual void DrawSurface(
		std::shared_ptr<IMeshResource> meshRes,
		std::shared_ptr<IMaterialResource> matRes,
		const Core::Math::Matrix& world) = 0;

	virtual void DrawUI(
		std::shared_ptr<IMeshResource> meshRes,
		std::shared_ptr<IMaterialResource> matRes,
		const Core::Math::Matrix& world,
		const Rect* source) = 0;
};
