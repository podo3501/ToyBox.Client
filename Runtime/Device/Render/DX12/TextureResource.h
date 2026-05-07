#pragma once
#include "GameClient/Service/Render/Repository/ITextureResource.h"
#include "GameClient/Service/Render/Repository/TextureDesc.h"
#include "DescriptorAllocation.h"
#include <wrl/client.h>
#include "d3dx12.h"

using Microsoft::WRL::ComPtr;

class TextureResource : public ITextureResource
{
public:
	~TextureResource();
	TextureResource() = default;
	virtual bool IsReady() const noexcept 
	{ 
		return m_ready; 
	}

	void SetResource(ComPtr<ID3D12Resource> resource) { m_texture = std::move(resource); }
	void SetSRV(DescriptorAllocation allocation) { m_srv = std::move(allocation); }
	DescriptorAllocation& GetSrv() { return m_srv; }
	void MarkReady() 
	{ 
		m_ready = true; 
	}

	ID3D12Resource* Get() const { return m_texture.Get(); }
	void Set(ComPtr<ID3D12Resource> tex) { m_texture = tex; }

private:
	ComPtr<ID3D12Resource> m_texture;
	DescriptorAllocation m_srv;
	bool m_ready{ false };
};