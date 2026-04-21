#pragma once
#include "GameClient/Service/Render/ITextureResource.h"
#include "GameClient/Service/Render/TextureDesc.h"
#include "PendingTransition.h"
#include "DescriptorAllocation.h"
#include "TaskHandle.h"
#include <wrl/client.h>
#include "d3dx12.h"

using Microsoft::WRL::ComPtr;

class CommandScheduler;
class DescriptorAllocator;
class ResourceUploader;
class TaskScheduler;
class ResourcePreparer;
class MipGenerator;
struct SubmittedFences;

class TextureResource : public ITextureResource, public IResourceReady
{
public:
	~TextureResource();
	TextureResource() = delete;
	TextureResource(ID3D12Device* device, CommandScheduler* command, DescriptorAllocator* srvAllocator, 
		TaskScheduler* taskScheduler, ResourceUploader* uploader, ResourcePreparer* preparer, MipGenerator* mipGenerator);
	virtual bool LoadFromAsset(shared_ptr<TextureAsset> asset, const TextureDesc& desc) override;
	virtual bool IsReady() const noexcept override { return m_ready; }
	virtual void OnReady(CommandList& cmd) override {};

	DescriptorAllocation& GetSrv() { return m_srv; }
	ID3D12Resource* Get() { return m_texture.Get(); }

private:
	void AddTexture(ComPtr<ID3D12Resource> tex, const TextureDesc& desc, bool generateMips);

	ID3D12Device* m_device{ nullptr };
	CommandScheduler* m_command{ nullptr };
	DescriptorAllocator* m_srvAllocator{ nullptr };
	TaskScheduler* m_taskScheduler{ nullptr };
	ResourceUploader* m_uploader{ nullptr };
	ResourcePreparer* m_preparer{ nullptr };
	MipGenerator* m_mipGenerator{ nullptr };

	DescriptorAllocation m_srv;
	bool m_ready{ false };
	ComPtr<ID3D12Resource> m_texture;
	//bool m_canGenerateMips{ false }; //실제로 mip을 만들수 있는가.
	bool m_mipGenerated{ false }; //두번 만드는거 방지용

	TaskHandle m_uploadHandle{};
	TaskHandle m_toSrvHandle{};
	TaskHandle m_toUavHandle{};
	TaskHandle m_mipHandle{};
	TaskHandle m_finalizeHandle{};
};
