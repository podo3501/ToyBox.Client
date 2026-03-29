#include "pch.h"
#include "RenderBackend.h"
#include "DeviceResources.h"
#include "Core/Foundation/Geometry2D.h"

RenderBackend::~RenderBackend() = default;
RenderBackend::RenderBackend() :
    m_deviceResources{ make_unique<DeviceResources>() }
{}

bool RenderBackend::Initialize(HWND hwnd, const Size& wndSize, const RenderConfig& renderConfig)
{
    if (!m_deviceResources->Initialize(hwnd, wndSize, renderConfig))
        return false;

	return true;
}

//////////////////////////////////////////////////////

unique_ptr<IRenderBackend> CreateRenderBackend()
{
	return make_unique<RenderBackend>();
}
