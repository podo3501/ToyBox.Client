#pragma once
#include "GameClient/Service/Render/IRenderBackend.h"
#include "Core/Foundation/Geometry2D.h"
#include <wrl/client.h>

struct MeshBuffer;
class DX12Core;
class SwapChainPresenter;
class CommandList;
class CommandScheduler;
class DescriptorAllocator;
class TaskScheduler;
class ResourceLoader;
class GPUProfiler;
class TextureSystem;
class MeshSystem;
class MaterialSystem;
class ShaderSystem;
class MeshRenderer;
class UIRenderer;
class RenderScene;

class RenderBackend : public IRenderBackend
{
public:
	~RenderBackend();
	RenderBackend();
	virtual bool Initialize(HWND hwnd, const Size& wndSize, const RenderConfig& renderConfig, const std::vector<ShaderRegisterDesc>& shaders) override;
	virtual void SetDirectionalLight(const DirectionalLightData& light) override;
	virtual void SetCamera(const CameraData& camera) override;
	virtual void DrawMesh(std::shared_ptr<IMeshResource> meshRes, std::shared_ptr<IMaterialResource> matRes,
		const Core::Math::Matrix& world) override;
	virtual void DrawUI(std::shared_ptr<IMeshResource> meshRes, std::shared_ptr<IMaterialResource> matRes,
		const Core::Math::Matrix& world) override;
	virtual void Resize(const Size& size) override;
	virtual void Update() override;
	virtual void Render() override;
	virtual void WaitIdle() override;
	virtual ITextureSystem* GetTextureSystem() override;
	virtual IMeshSystem* GetMeshSystem() override;
	virtual IMaterialSystem* GetMaterialSystem() override;

private:
	bool BeginFrame();
	void EndFrame();
	void Clear(CommandList& cmd, float r, float g, float b, float a);
	size_t ComputeTextureBudget(float gpuMs);
	size_t ComputeMeshBudget(float gpuMs);

	unique_ptr<DX12Core> m_core;
	unique_ptr<CommandScheduler> m_command;
	unique_ptr<SwapChainPresenter> m_swapChain;
	unique_ptr<DescriptorAllocator> m_srvAllocator;
	unique_ptr<TaskScheduler> m_taskScheduler;
	unique_ptr<ResourceLoader> m_loader;
	unique_ptr<GPUProfiler> m_profiler;
	
	unique_ptr<TextureSystem> m_texSystem;
	unique_ptr<MeshSystem> m_meshSystem;
	unique_ptr<MaterialSystem> m_matSystem;
	unique_ptr<ShaderSystem> m_shaderSystem;
	unique_ptr<RenderScene> m_scene;

	unique_ptr<MeshRenderer> m_meshRenderer;
	unique_ptr<UIRenderer> m_uiRenderer;

	CommandList* m_cmd{ nullptr }; //direct command юс.

	Size m_size{};
	DirectionalLightData m_lightData;
	CameraData m_cameraData;
};