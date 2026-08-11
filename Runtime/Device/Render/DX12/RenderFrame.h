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

	virtual void DrawText(
		std::shared_ptr<IResource> fontRes,
		TextRenderMode mode,
		std::span<const TextSpan> spans,
		uint32_t size,
		const Rect& bounds,
		const TextLayout& layout);

	virtual void DrawSurface(
		std::shared_ptr<IMeshResource> meshRes,
		std::shared_ptr<IMaterialResource> matRes,
		const Core::Matrix& world) override;

	virtual void DrawUI(
		std::shared_ptr<IMeshResource> meshRes,
		std::shared_ptr<IResource> brushRes,
		const Core::Matrix& world,
		const Rect* source) override;

	virtual void DrawEnvironment(std::shared_ptr<IResource> envRes) override;

	DrawPacket PrepareRenderData();
	void Clear();

	const FrameData& GetFrameData() const { return m_frameData; }

private:
	TextSystem& m_textSystem;
	Inspector& m_inspector;
	RenderScene m_scene;

	FrameData m_frameData;
	std::vector<DrawTextItem> m_pendingTexts;
};