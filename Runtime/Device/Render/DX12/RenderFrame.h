#pragma once
#include "GameClient/Service/Render/IRenderFrame.h"
#include "Scene/RenderScene.h"

class RenderFrame : public IRenderFrame
{
public:
	~RenderFrame();
	RenderFrame();

	virtual void SetFrameData(const FrameData& frameData) noexcept override;
	virtual void DrawText(
		std::shared_ptr<IFontResource> fontRes,
		std::string_view text,
		const Vector2& pos) override;
	virtual void DrawSurface(
		std::shared_ptr<IMeshResource> meshRes,
		std::shared_ptr<IMaterialResource> matRes,
		const Core::Math::Matrix& world) override;
	virtual void DrawUI(
		std::shared_ptr<IMeshResource> meshRes,
		std::shared_ptr<IMaterialResource> matRes,
		const Core::Math::Matrix& world,
		const Rect* source) override;

	DrawPacket PrepareRenderData();
	void Clear();

	const FrameData& GetFrameData() const { return m_frameData; }

private:
	RenderScene m_scene;
	FrameData m_frameData;
};