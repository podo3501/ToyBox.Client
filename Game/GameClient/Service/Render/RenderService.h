#pragma once

struct IRenderBackend;
struct IResourceManager;
struct Rect;
struct Size;
struct TextureAsset;

class RenderService
{
public:
	~RenderService();
	RenderService() = delete;
	static unique_ptr<RenderService> Create(unique_ptr<IRenderBackend> backend) noexcept;
	int AcquireTexture(const filesystem::path& path, function<shared_ptr<TextureAsset>(const filesystem::path&)> loader);
	void Draw(int index, const Rect& dest, const Rect* source);
	void Resize(const Size& size);
	void Update();

private:
	RenderService(unique_ptr<IRenderBackend> backend);

	unique_ptr<IRenderBackend> m_backend;
	unordered_map<filesystem::path, int> m_cache;
};