#pragma once
#include "TextureHandle.h"

struct IRenderBackend;
struct Rect;
struct Size;
struct TextureAsset;
struct DrawCommand;
class TextureRepository;

class RenderService
{
public:
	~RenderService();
	RenderService() = delete;
	static unique_ptr<RenderService> Create(unique_ptr<IRenderBackend> backend) noexcept;
	TextureHandle LoadTexture(const filesystem::path& path, function<shared_ptr<TextureAsset>(const filesystem::path&)> loader);
	bool ReleaseTexture(TextureHandle th);
	void Draw(TextureHandle th, const Rect& dest, const Rect* source);
	void Resize(const Size& size);
	void Update();

private:
	RenderService(unique_ptr<IRenderBackend> backend);
	void Flush();

	unique_ptr<IRenderBackend> m_backend;
	unique_ptr<TextureRepository> m_texRepository;
	vector<DrawCommand> m_drawQueue;
};