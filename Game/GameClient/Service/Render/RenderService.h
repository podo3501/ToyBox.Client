#pragma once
#include "Repository/RenderRepository.h"

struct IRenderBackend;
struct Rect;
struct Size;
struct DrawCommand;
class RenderRepository;

class RenderService
{
public:
	~RenderService();
	RenderService() = delete;
	static unique_ptr<RenderService> Create(unique_ptr<IRenderBackend> backend) noexcept;
	void Draw(TextureHandle th, const Rect& dest, const Rect* source);
	void Update();
	void Render();
	void Resize(const Size& size);

	RenderRepository* GetRepository();

private:
	RenderService(unique_ptr<IRenderBackend> backend);

	unique_ptr<IRenderBackend> m_backend;
	unique_ptr<RenderRepository> m_repository;
	vector<DrawCommand> m_drawQueue;
};