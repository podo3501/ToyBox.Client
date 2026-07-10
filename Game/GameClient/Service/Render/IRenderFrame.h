#pragma once
#include "GameClient/Graphics/RenderData/FrameData.h"
#include "GameClient/Service/Render/Resource/IFontResource.h"
#include "GameClient/Service/Render/Resource/IMeshResource.h"
#include "GameClient/Service/Render/Resource/IMaterialResource.h"
#include "Core/Foundation/Geometry2D.h"
#include "Core/Math/Vector2.h"

struct IRenderFrame
{
	virtual ~IRenderFrame() = default;
	virtual void SetFrameData(const FrameData& frameData) noexcept = 0;

	virtual void DrawText(
		std::shared_ptr<IMeshResource> meshRes,
		std::shared_ptr<IFontResource> fontRes,
		std::string_view text,
		uint32_t size,
		const Core::Math::Vector2& pos) = 0;

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
