#pragma once
#include "GameClient/Service/Render/IRenderBackend.h"
#include "Core/Foundation/Geometry2D.h"
#include <wrl/client.h>

struct ID3D12Resource;
struct ID3D12DescriptorHeap;
struct ID3D12GraphicsCommandList;
struct TextureEntry;
struct ImageData;
class DX12Core;
class SwapChainPresenter;
class CommandScheduler;
class QuadRenderer;
class TextureLoader;
class ResourceUploader;

class RenderBackend : public IRenderBackend
{
public:
	~RenderBackend();
	RenderBackend();
	virtual bool Initialize(HWND hwnd, const Size& wndSize, const RenderConfig& renderConfig) override;
	virtual int LoadTextureFromMemory(Core::ByteBuffer buffer) override;
	virtual void Draw(int index, const Rect& dest, const Rect* source) override;

private:
	ID3D12GraphicsCommandList* BeginFrame();
	void Clear(ID3D12GraphicsCommandList* cmd, float r, float g, float b, float a);
	bool EndFrame(ID3D12GraphicsCommandList* cmd);

	unique_ptr<DX12Core> m_core;
	unique_ptr<CommandScheduler> m_command;
	unique_ptr<SwapChainPresenter> m_swapChain;
	unique_ptr<QuadRenderer> m_quadRenderer;
	unique_ptr<TextureLoader> m_textureLoader;
	unique_ptr<ResourceUploader> m_uploader;

	Size m_size{};

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_srvHeap;
	UINT m_srvDescriptorSize = 0;
	vector<TextureEntry> m_textures;

	bool m_ready{ false };
};