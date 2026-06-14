#pragma once
#include "GameClient/Service/Render/IRenderBackend.h"
#include "Core/Device.h"
#include "Core/Foundation/Geometry2D.h"
#include <wrl/client.h>

class SwapChainPresenter;
class CommandList;
class CommandScheduler;
class DescriptorAllocator;
class DescriptorFactory;
class ResourceFactory;
class TaskScheduler;
class GPUProfiler;
class TextureProvider;
class MeshProvider;
class MaterialProvider;
class ShaderProvider;
class Renderers;
class RenderScene;
class ShadowResource;

class RenderBackend : public IRenderBackend
{
public:
	~RenderBackend();
	RenderBackend(const RenderConfig& config);
	virtual bool Initialize(HWND hwnd, const Size& wndSize, const std::vector<ShaderRegisterDesc>& shaders) override;
	virtual void SetDirectionalLight(const DirectionalLightData& light) override;
	virtual void SetCamera(const CameraData& camera) override;
	virtual void DrawSurface(
		std::shared_ptr<IMeshResource> meshRes,
		std::shared_ptr<IMaterialResource> matRes,
		const Core::Math::Matrix& world) override;
	virtual void DrawUI(
		std::shared_ptr<IMeshResource> meshRes, 
		std::shared_ptr<IMaterialResource> matRes,
		const Core::Math::Matrix& world) override;
	virtual void Resize(const Size& size) override;
	virtual void Update() override;
	virtual void Render() override;
	virtual void WaitIdle() override;
	virtual ITextureProvider* GetTextureProvider() override;
	virtual IMeshProvider* GetMeshProvider() override;
	virtual IMaterialProvider* GetMaterialProvider() override;

private:
	bool BeginFrame();
	void EndFrame();
	void Clear(CommandList& cmd, float r, float g, float b, float a);
	size_t ComputeTextureBudget(float gpuMs);
	size_t ComputeMeshBudget(float gpuMs);

	Device m_device;
	RenderConfig m_config;

	unique_ptr<CommandScheduler> m_command;
	unique_ptr<SwapChainPresenter> m_swapChain;
	unique_ptr<DescriptorAllocator> m_srvAllocator;
	unique_ptr<DescriptorAllocator> m_dsvAllocator;
	unique_ptr<DescriptorFactory> m_descFactory;
	unique_ptr<ResourceFactory> m_resFactory;
	unique_ptr<TaskScheduler> m_taskScheduler;
	unique_ptr<GPUProfiler> m_profiler;
	
	unique_ptr<TextureProvider> m_texProvider;
	unique_ptr<MeshProvider> m_meshProvider;
	unique_ptr<MaterialProvider> m_matProvider;
	unique_ptr<ShaderProvider> m_shaderProvider;
	unique_ptr<RenderScene> m_scene;
	unique_ptr<Renderers> m_renderers;
	unique_ptr<ShadowResource> m_shadowRes;

	CommandList* m_cmd{ nullptr }; //direct command юс.

	Size m_size{};
	DirectionalLightData m_lightData;
	CameraData m_cameraData;
};