#pragma once
#include "GameClient/Service/Render/IRenderBackend.h"

class DeviceResources;
class RenderBackend : public IRenderBackend
{
public:
	~RenderBackend();
	RenderBackend();
	virtual bool Initialize(HWND hwnd, const Size& wndSize, const RenderConfig& renderConfig) override;

private:
	unique_ptr<DeviceResources> m_deviceResources;
};