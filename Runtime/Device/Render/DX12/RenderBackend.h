#pragma once
#include "GameClient/Service/Render/IRenderBackend.h"
#include "Core/Foundation/Geometry2D.h"
#include <wrl/client.h>

struct ID3D12GraphicsCommandList;
struct MeshEntry;
struct QuadDrawInfo;
class DX12Core;
class SwapChainPresenter;
class CommandScheduler;
class DescriptorAllocator;
class ResourceUploader;
class ResourcePreparer;
class MipGenerator;
class QuadRenderer;

class RenderBackend : public IRenderBackend
{
public:
	~RenderBackend();
	RenderBackend();
	virtual bool Initialize(HWND hwnd, const Size& wndSize, const RenderConfig& renderConfig) override;
	virtual unique_ptr<ITextureResource> CreateTextureResource() override;
	virtual void BeginFrame() override;
	virtual void EndFrame() override;
	virtual void Draw(ITextureResource* texRes, const Rect& dest, const Rect* source) override;
	virtual void Resize(const Size& size) override;
	virtual void Update() override;

private:
	void Clear(ID3D12GraphicsCommandList* cmd, float r, float g, float b, float a);

	unique_ptr<DX12Core> m_core;
	unique_ptr<CommandScheduler> m_command;
	unique_ptr<SwapChainPresenter> m_swapChain;
	unique_ptr<DescriptorAllocator> m_srvAllocator;
	unique_ptr<ResourceUploader> m_uploader;
	unique_ptr<ResourcePreparer> m_preparer;
	unique_ptr<MipGenerator> m_mipGenerator;
	unique_ptr<QuadRenderer> m_quadRenderer;

	ID3D12GraphicsCommandList* m_graphicsCmdList{ nullptr }; //direct command юс.

	Size m_size{};
	vector<MeshEntry> m_meshes;
	bool m_ready{ false };
};