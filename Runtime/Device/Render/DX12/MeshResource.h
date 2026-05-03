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

	void SetResource(ComPtr<ID3D12Resource> vb, ComPtr<ID3D12Resource> ib, UINT indexCount);
	void SetSRV(DescriptorAllocation vbAlloc, DescriptorAllocation ibAlloc);
	void MarkReady() { m_ready = true; }
	UINT GetIndexCount() const { return m_indexCount; }
	DescriptorAllocation& GetVBSrv() { return m_vbSrv; }
	DescriptorAllocation& GetIBSrv() { return m_ibSrv; }

private:
	ComPtr<ID3D12Resource> m_vb;
	ComPtr<ID3D12Resource> m_ib;

	DescriptorAllocation m_vbSrv;
	DescriptorAllocation m_ibSrv;

	UINT m_indexCount{ 0 };
	bool m_ready{ false };
};