#pragma once
#include "GameClient/Graphics/RenderData/FrameData.h"
#include "GameClient/Service/Render/Resource/IResource.h"
#include "GameClient/Service/Render/Definition/View/SceneView.h"
#include "GameClient/Service/Render/Definition/Text/TextStyle.h"
#include "Core/Foundation/Geometry2D.h"
#include "Core/Foundation/Color.h"
#include "Core/Math/Vector2.h"

struct IRenderFrame
{
	virtual ~IRenderFrame() = default;
	virtual void SetFrameData(const FrameData& frameData) noexcept = 0;

	virtual ViewDrawList& BeginView(const ViewContext& view) = 0;
	virtual void EndView() = 0;

	virtual void DrawText(
		std::shared_ptr<IResource> fontRes,
		TextRenderMode mode,
		uint32_t size,
		const Rect& bounds,
		const TextLayout& layout,
		std::vector<TextRun> textRuns) = 0;

	virtual void DrawSurface(
		std::shared_ptr<IResource> meshRes,
		std::shared_ptr<IResource> matRes,
		std::optional<ShaderID> shaderOverride,
		const Core::Matrix& world) = 0;

	virtual void DrawDebugSurface(
		std::shared_ptr<IResource> meshRes,
		std::shared_ptr<IResource> matRes,
		const Core::Matrix& world) = 0;

	virtual void DrawUI(
		std::shared_ptr<IResource> meshRes,
		std::shared_ptr<IResource> brushRes,
		const Core::Matrix& world,
		const Rect* source) = 0;

	virtual void DrawEnvironment(std::shared_ptr<IResource> envRes) = 0;
};
