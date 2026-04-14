#pragma once
#include "GameClient/Service/Render/ITextureResource.h"
#include <wrl/client.h>
#include "d3dx12.h"

using Microsoft::WRL::ComPtr;

class DescriptorAllocator;
class ResourceUploader;
class CommandScheduler;

class TextureResource : public ITextureResource
{
public:
	~TextureResource();
	TextureResource() = delete;
	TextureResource(ID3D12Device* device, CommandScheduler* command,
		DescriptorAllocator* srvAllocator, ResourceUploader* uploader);
	virtual bool LoadFromAsset(shared_ptr<TextureAsset> asset) override;

	ID3D12Resource* Get() const noexcept { return m_tex.Get(); };
	UINT GetSrvIndex() const noexcept { return m_srvIndex; }

private:
	void AddTexture(UINT index, ComPtr<ID3D12Resource> tex);

	ID3D12Device* m_device{ nullptr };
	CommandScheduler* m_command{ nullptr };
	DescriptorAllocator* m_srvAllocator{ nullptr };
	ResourceUploader* m_uploader{ nullptr };

	ComPtr<ID3D12Resource> m_tex{ nullptr };
	UINT m_srvIndex{ UINT_MAX };
};
