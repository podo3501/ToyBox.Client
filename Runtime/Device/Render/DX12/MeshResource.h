#pragma once
#include "GameClient/Service/Render/Resource/IMeshResource.h"
#include "Descriptor/DescriptorAllocation.h"
#include <wrl/client.h>
#include "d3dx12.h"

using Microsoft::WRL::ComPtr;

class MeshResource : public IMeshResource
{
public:
	~MeshResource();
	MeshResource();
	virtual bool IsReady() const noexcept override { return m_ready; }
	virtual VertexFormat GetVertexFormat() const noexcept override { return m_format; }

	void SetVertexFormat(VertexFormat format) noexcept { m_format = format; }
	void SetResource(ComPtr<ID3D12Resource> vb, ComPtr<ID3D12Resource> ib, 
		UINT vertexCount, UINT indexCount) noexcept;
	void SetMeshTable(DescriptorAllocation table) noexcept;
	void MarkReady() noexcept { m_ready = true; }
	UINT GetVertexCount() const noexcept { return m_vertexCount; }
	UINT GetIndexCount() const noexcept { return m_indexCount; }
	DescriptorAllocation& GetMeshTable() noexcept { return m_meshTable; }

private:
	VertexFormat m_format;
	ComPtr<ID3D12Resource> m_vb;
	ComPtr<ID3D12Resource> m_ib;

	DescriptorAllocation m_meshTable;

	UINT m_vertexCount{ 0 };
	UINT m_indexCount{ 0 };
	bool m_ready{ false };
};