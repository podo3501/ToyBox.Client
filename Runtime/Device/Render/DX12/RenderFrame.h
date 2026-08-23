#pragma once
#include "GameClient/Service/Render/IRenderFrame.h"

struct FramePacket;
class TextSystem;
class Inspector;

class RenderFrame : public IRenderFrame
{
public:
	~RenderFrame();
	RenderFrame(TextSystem& textSystem, Inspector& inspector);

	virtual void SubmitFrame(SceneFrameData frame) noexcept override;

	FramePacket PrepareRenderData(const Size& screenSize);

private:
	TextSystem& m_textSystem;
	Inspector& m_inspector;

	SceneFrameData m_pendingFrame;
};