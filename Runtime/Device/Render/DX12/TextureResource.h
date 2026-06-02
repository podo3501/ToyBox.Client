#pragma once
#include "GameClient/Service/Render/Resource/ITextureResource.h"
#include "GameClient/Service/Render/Desc/TextureDesc.h"
#include <wrl/client.h>
#include "d3dx12.h"

using Microsoft::WRL::ComPtr;

class TextureResource : public ITextureResource
{
public:
	~TextureResource();
	TextureResource() = delete;
	TextureResource(const TextureDesc& desc);
	virtual bool IsReady() const noexcept { return m_ready; }
	void MarkReady() { m_ready = true; }

	const TextureDesc& GetDesc() const noexcept { return m_desc; }
	ID3D12Resource* Get() const { return m_texture.Get(); }
	void Set(ComPtr<ID3D12Resource> tex) { m_texture = tex; }
	void SetResource(ComPtr<ID3D12Resource> resource) { m_texture = std::move(resource); }

	void SetHeapIndex(UINT index) noexcept { m_heapIndex = index; }
	UINT GetHeapIndex() const noexcept { return m_heapIndex; }

	void SetMipSrvIndices(std::vector<UINT>&& indices) 
	{ 
		m_mipSrvIndices = std::move(indices); 
	}
	UINT GetMipSrvIndex(UINT mipLevel) const noexcept
	{
		return (mipLevel < m_mipSrvIndices.size()) ? m_mipSrvIndices[mipLevel] : UINT_MAX;
	}

	void SetMipUavIndices(std::vector<UINT>&& indices) { m_mipUavIndices = std::move(indices); }
	UINT GetMipUavIndex(UINT mipLevel) const noexcept
	{
		return (mipLevel < m_mipUavIndices.size()) ? m_mipUavIndices[mipLevel] : UINT_MAX;
	}
	

private:
	TextureDesc m_desc;
	ComPtr<ID3D12Resource> m_texture;
	UINT m_heapIndex{ UINT_MAX };
	bool m_ready{ false };

	std::vector<UINT> m_mipSrvIndices;
	std::vector<UINT> m_mipUavIndices;
};