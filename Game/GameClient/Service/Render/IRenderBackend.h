#pragma once
#include "RenderConfig.h"

struct Size;
struct Rect;

struct IRenderBackend
{
	virtual ~IRenderBackend() = default;
	virtual bool Initialize(HWND hwnd, const Size& wndSize, const RenderConfig& config) = 0;
	virtual bool BeginFrame() = 0;
	virtual void Clear(float r, float g, float b, float a) = 0;
	virtual bool EndFrame() = 0;
	virtual bool Render() = 0;

	virtual int LoadTextureFromMemory(Core::ByteBuffer buffer) = 0;
	virtual void Draw(int index, const Rect& dest, const Rect* source) = 0;
};

unique_ptr<IRenderBackend> CreateRenderBackend();