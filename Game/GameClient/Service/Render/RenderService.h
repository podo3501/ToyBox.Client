#pragma once
#include "Core/Utils/Handle/HandlePool.h"
#include "TextureHandle.h"

struct IRenderBackend;
struct IResourceManager;
struct ITextureResource;
struct Rect;
struct Size;
struct TextureAsset;
struct DrawCommand;

class RenderService
{
public:
	~RenderService();
	RenderService() = delete;
	static unique_ptr<RenderService> Create(unique_ptr<IRenderBackend> backend) noexcept;
	TextureHandle AcquireTexture(const filesystem::path& path, function<shared_ptr<TextureAsset>(const filesystem::path&)> loader);
	bool ReleaseTexture(TextureHandle th);
	void Draw(TextureHandle th, const Rect& dest, const Rect* source);
	void Resize(const Size& size);
	void Update();

private:
	RenderService(unique_ptr<IRenderBackend> backend);
	shared_ptr<ITextureResource> CreateTextureResource(shared_ptr<TextureAsset> asset);
	void Flush();

	unique_ptr<IRenderBackend> m_backend;
	unordered_map<filesystem::path, weak_ptr<ITextureResource>> m_cache;
	HandlePool<shared_ptr<ITextureResource>, TextureTag> m_loadedTextures;
	std::vector<DrawCommand> m_drawQueue;
};