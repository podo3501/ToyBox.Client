#pragma once
#include "GameClient/Service/Render/ITextureResource.h"
#include "PendingTransition.h"
#include <wrl/client.h>
#include "d3dx12.h"

using Microsoft::WRL::ComPtr;

class CommandScheduler;
class DescriptorAllocator;
class ResourceUploader;
class ResourcePreparer;

class TextureResource : public ITextureResource, public IResourceReady
{
public:
	~TextureResource();
	TextureResource() = delete;
	TextureResource(ID3D12Device* device, CommandScheduler* command,
		DescriptorAllocator* srvAllocator, ResourceUploader* uploader, ResourcePreparer* preparer);
	virtual bool LoadFromAsset(shared_ptr<TextureAsset> asset, const TextureDesc& desc) override;
	virtual bool IsReady() const noexcept override { return m_ready; }
	virtual void OnReady() override { m_ready = true; }

	UINT GetSrvIndex() const noexcept { return m_srvIndex; }

private:
	void AddTexture(UINT index, ComPtr<ID3D12Resource> tex, const TextureDesc& desc);

	ID3D12Device* m_device{ nullptr };
	CommandScheduler* m_command{ nullptr };
	DescriptorAllocator* m_srvAllocator{ nullptr };
	ResourceUploader* m_uploader{ nullptr };
	ResourcePreparer* m_preparer{ nullptr };

	UINT m_srvIndex{ UINT_MAX };
	bool m_ready{ false };
};
