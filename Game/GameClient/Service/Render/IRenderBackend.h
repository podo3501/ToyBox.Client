#pragma once
#include "RenderConfig.h"

struct Size;
struct Rect;
struct TextureAsset;

struct IRenderBackend
{
	virtual ~IRenderBackend() = default;
	virtual bool Initialize(HWND hwnd, const Size& wndSize, const RenderConfig& config) = 0;
	virtual int UploadTexture(const TextureAsset& asset) = 0;
	virtual void Draw(int index, const Rect& dest, const Rect* source) = 0;
	virtual void Resize(const Size& size) = 0;
	virtual void Update() = 0;
};

unique_ptr<IRenderBackend> CreateRenderBackend();