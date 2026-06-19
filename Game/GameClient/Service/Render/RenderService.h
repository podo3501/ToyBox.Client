#pragma once
#include "RenderContext.h"
#include "RenderServiceConfig.h"
#include "../AssetAsync/AssetAsyncTypes.h"

struct IRenderBackend;
struct FrameData;
struct Asset;
class RenderContext;

class RenderService
{
public:
	~RenderService();
	RenderService() = delete;
	static unique_ptr<RenderService> Create(
		unique_ptr<IRenderBackend> backend,
		AssetPipelineT* assetPipeline,
		const DefaultMaterialDescs& defaultMatDescs) noexcept;

	bool Initialize(const DefaultMaterialDescs& defaultMatDescs);
	void SetFrameData(const FrameData& frameData);
	void Update();
	void Render();
	void Resize(const Size& size);

	RenderContext* GetContext();

private:
	RenderService(unique_ptr<IRenderBackend> backend, AssetPipelineT* assetPipeline);

	unique_ptr<IRenderBackend> m_backend;
	unique_ptr<RenderContext> m_context;
};