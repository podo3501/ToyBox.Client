#pragma once
#include "GameClient/Service/Render/IRenderFrame.h"

struct RenderPacket;
struct RenderTextItem;
class TextSystem;
class Inspector;

class RenderFrame : public IRenderFrame
{
public:
	~RenderFrame();
	RenderFrame(TextSystem& textSystem, Inspector& inspector);

	virtual void SetFrameData(const FrameData& frameData) noexcept override;
	virtual void SubmitViews(std::vector<SceneViewData> views) override;

	void Clear();
	std::vector<std::shared_ptr<RenderPacket>> PrepareRenderData();
	const FrameData& GetFrameData() const { return m_frameData; }

private:
	TextSystem& m_textSystem;
	Inspector& m_inspector;
	FrameData m_frameData;
	std::vector<SceneViewData> m_pendingViews;
};