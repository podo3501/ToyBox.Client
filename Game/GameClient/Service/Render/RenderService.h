#pragma once

struct IRenderBackend;

class RenderService
{
public:
	~RenderService();
	RenderService() = delete;
	static unique_ptr<RenderService> Create(unique_ptr<IRenderBackend> backend) noexcept;
	void RenderFrame();

private:
	RenderService(unique_ptr<IRenderBackend> backend);

	unique_ptr<IRenderBackend> m_backend;
};