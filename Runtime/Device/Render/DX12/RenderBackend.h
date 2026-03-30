#pragma once
#include "GameClient/Service/Render/IRenderBackend.h"
#include "Core/Foundation/Geometry2D.h"

class DX12Device;
class FrameDisplay;
class CommandScheduler;
class QuadRenderer;

class RenderBackend : public IRenderBackend
{
public:
	~RenderBackend();
	RenderBackend();
	virtual bool Initialize(HWND hwnd, const Size& wndSize, const RenderConfig& renderConfig) override;
	virtual bool BeginFrame() override;
	virtual void Clear(float r, float g, float b, float a) override;
	virtual bool EndFrame() override;
	//virtual bool Present(bool vsync) override;
	virtual bool Render() override;

private:
	unique_ptr<DX12Device> m_device;
	unique_ptr<CommandScheduler> m_commandScheduler;
	unique_ptr<FrameDisplay> m_frameDisplay;
	unique_ptr<QuadRenderer> m_quadRenderer;

	Size m_size{};
};