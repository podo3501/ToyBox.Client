#pragma once
#include "Core/Foundation/Geometry2D.h"
#include "RenderServiceConfig.h"
#include "RenderMetrics.h"
#include "RenderRepository.h"
#include "SceneRenderer.h"
#include "../AssetAsync/AssetAsyncTypes.h"

struct IRenderBackend;
struct FrameData;
struct AssetData;
class AssetPipeline;
class MeshRepository;
class MaterialRepository;

class RenderService
{
public:
	~RenderService();
	RenderService() = delete;
	static unique_ptr<RenderService> Create(
		unique_ptr<IRenderBackend> backend,
		AssetPipeline* assetPipeline) noexcept;

	bool Initialize(
		HWND hwnd,
		const Size& screenSize,
		std::span<const BuiltinShaderDesc> builtinShaders,
		const DefaultMaterialDescs& defaultMatDescs);

	ShaderKey RegisterShader(const ShaderDesc& desc);
	void SetFrameData(const FrameData& frameData);
	void Update();
	void Render();
	void Resize(const Size& size);

	RenderRepository* GetRepository() { return m_repository.get(); }
	SceneRenderer* GetRenderer() { return m_renderer.get(); }
	RenderMetrics GetRenderMetrics();

private:
	RenderService(unique_ptr<IRenderBackend> backend, AssetPipeline* assetPipeline);

	unique_ptr<IRenderBackend> m_backend;

	unique_ptr<MeshRepository> m_meshRepository;
	unique_ptr<MaterialRepository> m_matRepository;

	unique_ptr<RenderRepository> m_repository;
	unique_ptr<SceneRenderer> m_renderer;
};