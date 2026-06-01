#pragma once
#include "GameClient/Service/Render/Resource/IMeshResource.h"
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

	void SetVertexHeapIndex(UINT heapIndex) noexcept { m_vertexHeapIndex = heapIndex; }
	void SetIndexHeapIndex(UINT heapIndex) noexcept { m_indexHeapIndex = heapIndex; }
	UINT GetVertexHeapIndex() const noexcept { return m_vertexHeapIndex; }
	UINT GetIndexHeapIndex() const noexcept { return m_indexHeapIndex; }

	void MarkReady() noexcept { m_ready = true; }
	UINT GetVertexCount() const noexcept { return m_vertexCount; }
	UINT GetIndexCount() const noexcept { return m_indexCount; }

private:
	VertexFormat m_format;
	ComPtr<ID3D12Resource> m_vb;
	ComPtr<ID3D12Resource> m_ib;

	UINT m_vertexCount{ 0 };
	UINT m_indexCount{ 0 };

	UINT m_vertexHeapIndex{ UINT_MAX };
	UINT m_indexHeapIndex{ UINT_MAX };

	bool m_ready{ false };
};