#pragma once
#include "GameClient/Service/Render/Repository/IMeshResource.h"
#include "DescriptorAllocation.h"
#include <wrl/client.h>
#include "d3dx12.h"

using Microsoft::WRL::ComPtr;

class MeshResource : public IMeshResource
{
public:
	~MeshResource();
	MeshResource();
	virtual bool IsReady() const noexcept { return m_ready; }

	void SetResource(
		ComPtr<ID3D12Resource> vb, UINT vbSize, UINT stride,
		ComPtr<ID3D12Resource> ib, UINT indexCount, DXGI_FORMAT indexFormat);

	const D3D12_VERTEX_BUFFER_VIEW& GetVBView() const { return m_vbView; }
	const D3D12_INDEX_BUFFER_VIEW& GetIBView() const { return m_ibView; }
	UINT GetIndexCount() const { return m_indexCount; }
	void MarkReady() { m_ready = true; }

	//void SetResource(ComPtr<ID3D12Resource> resource) { m_texture = std::move(resource); }
	//void SetSRV(DescriptorAllocation allocation) { m_srv = std::move(allocation); }
	//DescriptorAllocation& GetSrv() { return m_srv; }
	//ID3D12Resource* Get() const { return m_texture.Get(); }
	//void Set(ComPtr<ID3D12Resource> tex) { m_texture = tex; }

private:
	//DescriptorAllocation m_srv;

	ComPtr<ID3D12Resource> m_vb;
	ComPtr<ID3D12Resource> m_ib;

	D3D12_VERTEX_BUFFER_VIEW m_vbView{};
	D3D12_INDEX_BUFFER_VIEW m_ibView{};

	UINT m_indexCount{ 0 };

	bool m_ready{ false };
};