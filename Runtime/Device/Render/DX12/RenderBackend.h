#pragma once
#include "GameClient/Service/Render/IRenderBackend.h"
#include "Core/Foundation/Geometry2D.h"
#include <wrl/client.h>

struct MeshEntry;
struct QuadDrawInfo;
class DX12Core;
class SwapChainPresenter;
class CommandList;
class CommandScheduler;
class DescriptorAllocator;
class TaskScheduler;
class ResourceUploader;
class ResourcePreparer;
class TextureRegistry;
class DescriptorFactory;
class TextureGraphBuilder;
class TextureSystem;
class MipGenerator;
class QuadRenderer;

class RenderBackend : public IRenderBackend
{
public:
	~RenderBackend();
	RenderBackend();
	virtual bool Initialize(HWND hwnd, const Size& wndSize, const RenderConfig& renderConfig) override;
	//virtual shared_ptr<ITextureResource> CreateTextureResource() override;
	virtual void BeginFrame() override;
	virtual void EndFrame() override;
	virtual void Draw(ITextureResource* texRes, const Rect& dest, const Rect* source) override;
	virtual void Resize(const Size& size) override;
	virtual void Update() override;
	virtual ITextureSystem* GetTextureSystem() override;

private:
	void Clear(CommandList& cmd, float r, float g, float b, float a);

	unique_ptr<DX12Core> m_core;
	unique_ptr<CommandScheduler> m_command;
	unique_ptr<SwapChainPresenter> m_swapChain;
	unique_ptr<DescriptorAllocator> m_srvAllocator;
	unique_ptr<TaskScheduler> m_taskScheduler;
	unique_ptr<ResourceUploader> m_uploader;
	
	unique_ptr<ResourcePreparer> m_preparer;
	unique_ptr<MipGenerator> m_mipGenerator;

	unique_ptr<TextureRegistry> m_texRegistry;
	unique_ptr<DescriptorFactory> m_descriptorFactory;
	unique_ptr<TextureGraphBuilder> m_texGraphBuilder;
	unique_ptr<TextureSystem> m_texSystem;

	unique_ptr<QuadRenderer> m_quadRenderer;

	CommandList* m_cmd{ nullptr }; //direct command юс.

	Size m_size{};
	vector<MeshEntry> m_meshes;
	bool m_ready{ false };
};