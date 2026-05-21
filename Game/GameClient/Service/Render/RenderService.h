#pragma once
#include "RenderContext.h"

struct IRenderBackend;
struct DirectionalLightData;
struct CameraData;
class RenderContext;

class RenderService
{
public:
	~RenderService();
	RenderService() = delete;
	static unique_ptr<RenderService> Create(unique_ptr<IRenderBackend> backend) noexcept;
	bool Initialize();
	void SetCamera(const CameraData& camera);
	void SetDirectionalLight(const DirectionalLightData& light);
	void Update();
	void Render();
	void Resize(const Size& size);

	RenderContext* GetContext();

private:
	RenderService(unique_ptr<IRenderBackend> backend);

	unique_ptr<IRenderBackend> m_backend;
	unique_ptr<RenderContext> m_context;
};