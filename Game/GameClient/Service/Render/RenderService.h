#pragma once
#include "TextureHandle.h"
#include "MaterialHandle.h"
#include "TextureDesc.h"

struct IRenderBackend;
struct Rect;
struct Size;
struct TextureAsset;
struct DrawCommand;
class TextureRepository;
class MaterialRepository;

class RenderService
{
public:
	~RenderService();
	RenderService() = delete;
	static unique_ptr<RenderService> Create(unique_ptr<IRenderBackend> backend) noexcept;
	TextureHandle LoadTexture(const filesystem::path& path, const TextureDesc& desc, 
		function<shared_ptr<TextureAsset>(const filesystem::path&)> loader);
	bool ReleaseTexture(TextureHandle th);
	MaterialHandle CreateMaterial(TextureHandle th);
	void Draw(TextureHandle th, const Rect& dest, const Rect* source);
	void Update();
	void Render();
	void Resize(const Size& size);

private:
	RenderService(unique_ptr<IRenderBackend> backend);

	unique_ptr<IRenderBackend> m_backend;
	unique_ptr<TextureRepository> m_texRepository;
	unique_ptr<MaterialRepository> m_matRepository;
	vector<DrawCommand> m_drawQueue;
};