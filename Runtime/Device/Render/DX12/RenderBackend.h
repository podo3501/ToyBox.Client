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
class MeshRenderer;
class QuadRenderer;

class RenderBackend : public IRenderBackend
{
public:
	~RenderBackend();
	RenderBackend();
	virtual bool Initialize(HWND hwnd, const Size& wndSize, const RenderConfig& renderConfig) override;
	virtual void BeginFrame() override;
	virtual void EndFrame() override;
	virtual void Draw(ITextureResource* texRes, const Rect& dest, const Rect* source) override;
	virtual void DrawMesh(IMeshResource* meshRes) override;
	virtual void Resize(const Size& size) override;
	virtual void Update() override;
	virtual ITextureSystem* GetTextureSystem() override;
	virtual IMeshSystem* GetMeshSystem() override;

private:
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

	unique_ptr<MeshRenderer> m_meshRenderer;
	unique_ptr<QuadRenderer> m_quadRenderer;

	CommandList* m_cmd{ nullptr }; //direct command юс.

	Size m_size{};
	vector<MeshBuffer> m_meshes;
	bool m_ready{ false };
};