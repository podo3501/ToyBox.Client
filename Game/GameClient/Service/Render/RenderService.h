#pragma once

struct IRenderBackend;
struct IResourceManager;
struct Rect;
struct Size;

class RenderService
{
public:
	~RenderService();
	RenderService() = delete;
	static unique_ptr<RenderService> Create(unique_ptr<IRenderBackend> backend, IResourceManager* resManager) noexcept;
	int LoadTexture(const filesystem::path& filePath);
	void Draw(int index, const Rect& dest, const Rect* source);
	void Resize(const Size& size);
	void Update();

private:
	RenderService(unique_ptr<IRenderBackend> backend, IResourceManager* resManager);

	unique_ptr<IRenderBackend> m_backend;
	IResourceManager* m_resManager{ nullptr };
};