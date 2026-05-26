#pragma once
#include "RenderContext.h"
#include "../AssetAsync/AssetAsyncTypes.h"

struct IRenderBackend;
struct DirectionalLightData;
struct CameraData;
struct Asset;
class RenderContext;

class RenderService
{
public:
	~RenderService();
	RenderService() = delete;
	static unique_ptr<RenderService> Create(unique_ptr<IRenderBackend> backend, AssetPipelineT* assetPipeline) noexcept;
	bool Initialize();
	void SetCamera(const CameraData& camera);
	void SetDirectionalLight(const DirectionalLightData& light);
	void Update();
	void Render();
	void Resize(const Size& size);

	RenderContext* GetContext();

private:
	RenderService(unique_ptr<IRenderBackend> backend, AssetPipelineT* assetPipeline);

	unique_ptr<IRenderBackend> m_backend;
	unique_ptr<RenderContext> m_context;
};