#pragma once
#include "GameClient/Service/Render/IRenderBackend.h"
#include "Core/Foundation/Geometry2D.h"
#include <wrl/client.h>

struct ID3D12Resource;
struct ID3D12DescriptorHeap;
struct TextureEntry;
class DX12Device;
class SwapChainPresenter;
class CommandScheduler;
class QuadRenderer;

class RenderBackend : public IRenderBackend
{
public:
	~RenderBackend();
	RenderBackend();
	virtual bool Initialize(HWND hwnd, const Size& wndSize, const RenderConfig& renderConfig) override;
	virtual bool BeginFrame() override;
	virtual void Clear(float r, float g, float b, float a) override;
	virtual bool EndFrame() override;
	virtual bool Render() override;

	virtual int LoadTextureFromMemory(Core::ByteBuffer buffer) override;
	virtual void Draw(int index, const Rect& dest, const Rect* source) override;

private:
	void PrepareRender();
	void PreparePresent();
	Microsoft::WRL::ComPtr<ID3D12Resource> LoadFromMemory(Core::ByteBuffer buffer);

	unique_ptr<DX12Device> m_device;
	unique_ptr<SwapChainPresenter> m_swapChain;
	unique_ptr<CommandScheduler> m_command;
	unique_ptr<QuadRenderer> m_quadRenderer;

	Size m_size{};

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_srvHeap;
	UINT m_srvDescriptorSize = 0;
	vector<TextureEntry> m_textures;
};