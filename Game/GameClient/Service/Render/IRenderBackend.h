#pragma once
#include "RenderConfig.h"

struct Size;

struct IRenderBackend
{
	virtual ~IRenderBackend() = default;
	virtual bool Initialize(HWND hwnd, const Size& wndSize, const RenderConfig& config) = 0;
};

unique_ptr<IRenderBackend> CreateRenderBackend();