#pragma once
#include "GameClient/Service/Render/IRenderFrame.h"
#include "Scene/RenderScene.h"

class TextSystem;

class RenderFrame : public IRenderFrame
{
public:
	~RenderFrame();
	RenderFrame(TextSystem& textSystem);

	virtual void SetFrameData(const FrameData& frameData) noexcept override;
	virtual void DrawText(
		std::shared_ptr<IMeshResource> meshRes,
		std::shared_ptr<IFontResource> fontRes,
		std::string_view text,
		uint32_t size,
		const Core::Vector2& pos,
		const Core::Color& color) override;
	virtual void DrawSurface(
		std::shared_ptr<IMeshResource> meshRes,
		std::shared_ptr<IMaterialResource> matRes,
		const Core::Matrix& world) override;
	virtual void DrawUI(
		std::shared_ptr<IMeshResource> meshRes,
		std::shared_ptr<IMaterialResource> matRes,
		const Core::Matrix& world,
		const Rect* source) override;

	DrawPacket PrepareRenderData();
	void Clear();

	const FrameData& GetFrameData() const { return m_frameData; }

private:
	TextSystem& m_textSystem;
	RenderScene m_scene;
	FrameData m_frameData;
};