#pragma once
#include "GameClient/Service/Render/IRenderBackend.h"
#include "Core/Foundation/Geometry2D.h"
#include <wrl/client.h>

struct ID3D12GraphicsCommandList;
struct MeshEntry;
class DX12Core;
class SwapChainPresenter;
class CommandScheduler;
class DescriptorAllocator;
class QuadRenderer;
class TextureRepository;
class ResourceUploader;

class RenderBackend : public IRenderBackend
{
public:
	~RenderBackend();
	RenderBackend();
	virtual bool Initialize(HWND hwnd, const Size& wndSize, const RenderConfig& renderConfig) override;
	virtual int LoadTextureFromMemory(Core::ByteBuffer buffer) override;
	virtual void Draw(int index, const Rect& dest, const Rect* source) override;
	virtual void Update() override;
private:
	ID3D12GraphicsCommandList* BeginFrame();
	void Clear(ID3D12GraphicsCommandList* cmd, float r, float g, float b, float a);
	bool EndFrame(ID3D12GraphicsCommandList* cmd);

	unique_ptr<DX12Core> m_core;
	unique_ptr<CommandScheduler> m_command;
	unique_ptr<SwapChainPresenter> m_swapChain;
	unique_ptr<DescriptorAllocator> m_srvAllocator;
	unique_ptr<ResourceUploader> m_uploader;
	unique_ptr<TextureRepository> m_textureRepository;
	unique_ptr<QuadRenderer> m_quadRenderer;
	
	
	Size m_size{};

	vector<MeshEntry> m_meshes;

	bool m_ready{ false };
};