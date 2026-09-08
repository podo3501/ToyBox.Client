#pragma once
#include "RenderView.h"
#include "Handle/ResourceHandles.h"
#include "Definition/View/Overlay/OverlayViewData.h"
#include "Definition/Text/TextStyle.h"
#include "Core/Foundation/Geometry2D.h"

class Camera;
class RepositoryContainer;

class OverlayView : public RenderView
{
public:
	~OverlayView();
	OverlayView(
		RepositoryContainer& repositories,
		MeshHandle uiQuad,
		BrushHandle defaultBrush);
	virtual bool IsEmpty() const override;

	void Reset(
		const OverlayViewContext& context,
		const Camera& camera,
		const Size& screenSize);
	
	OverlayViewData TakeData();

	void DrawUI(
		BrushHandle bh,
		const Rect& dest,
		const Rect* source = nullptr);

	void DrawText(
		FontHandle hF,
		TextRenderMode mode,
		std::string_view text,
		uint32_t size,
		const Rect& bounds,
		const TextLayout& layout = {},
		const TextStyle& style = {});

	void DrawText(
		FontHandle hF,
		TextRenderMode mode,
		std::span<const TextSpan> spans,
		uint32_t size,
		const Rect& bounds,
		const TextLayout& layout);

private:
	MeshHandle m_uiQuad;
	BrushHandle m_defaultBrush;
	OverlayViewData m_data;
};