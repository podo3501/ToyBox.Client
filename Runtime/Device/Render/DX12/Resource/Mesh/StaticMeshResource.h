#pragma once
#include "MeshResource.h"
#include "Resource/Resource.h"
#include "GameClient/Asset/MeshAsset.h"

class StaticMeshResource : public MeshResource
{
public:
	virtual ~StaticMeshResource() override;
	StaticMeshResource();

	virtual UINT GetVertexHeapIndex() const noexcept override { return m_vertexHeapIndex; }
	virtual UINT GetIndexHeapIndex() const noexcept override { return m_indexHeapIndex; }
	virtual UINT GetVertexCount() const noexcept override { return m_vertexCount; }
	virtual UINT GetIndexCount() const noexcept override { return m_indexCount; }

	void SetVertexFormat(VertexFormat format) noexcept { m_format = format; }
	void SetResource(Resource vb, Resource ib, UINT vertexCount, UINT indexCount) noexcept;

	void SetVertexHeapIndex(UINT heapIndex) noexcept { m_vertexHeapIndex = heapIndex; }
	void SetIndexHeapIndex(UINT heapIndex) noexcept { m_indexHeapIndex = heapIndex; }

private:
	VertexFormat m_format;
	Resource m_vb;
	Resource m_ib;

	UINT m_vertexCount{ 0 };
	UINT m_indexCount{ 0 };

	UINT m_vertexHeapIndex{ UINT_MAX };
	UINT m_indexHeapIndex{ UINT_MAX };
};

