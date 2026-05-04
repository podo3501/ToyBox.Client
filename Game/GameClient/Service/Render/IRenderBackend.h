#pragma once
#include "RenderConfig.h"
#include "GameClient/Service/Render/Repository/ITextureSystem.h"
#include "GameClient/Service/Render/Repository/IMeshSystem.h"

struct ITextureResource;
struct IMeshResource;
struct TextureAsset;
struct Size;
struct Rect;

struct IRenderBackend
{
	virtual ~IRenderBackend() = default;
	virtual bool Initialize(HWND hwnd, const Size& wndSize, const RenderConfig& config) = 0;
	virtual void BeginFrame() = 0;
	virtual void EndFrame() = 0;
	virtual void Draw(ITextureResource* texRes, const Rect& dest, const Rect* source) = 0;
	virtual void DrawMesh(IMeshResource* meshRes) = 0;
	virtual void Resize(const Size& size) = 0;
	virtual void Update() = 0;
	virtual ITextureSystem* GetTextureSystem() = 0;
	virtual IMeshSystem* GetMeshSystem() = 0;
};

unique_ptr<IRenderBackend> CreateRenderBackend();