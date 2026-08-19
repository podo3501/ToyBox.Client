#pragma once
#include "GameClient/Service/Render/IRenderFrame.h"
#include "Scene/RenderScene.h"

struct DrawTextItem;
class TextSystem;
class Inspector;

class RenderFrame : public IRenderFrame
{
public:
	~RenderFrame();
	RenderFrame(TextSystem& textSystem, Inspector& inspector);

	virtual void SetFrameData(const FrameData& frameData) noexcept override;
	virtual void SubmitViews(std::vector<SceneViewData> views) override;

	DrawPacket PrepareRenderData();
	void Clear();

	const FrameData& GetFrameData() const { return m_frameData; }

private:
	TextSystem& m_textSystem;
	Inspector& m_inspector;
	RenderScene m_scene;

	FrameData m_frameData;
};