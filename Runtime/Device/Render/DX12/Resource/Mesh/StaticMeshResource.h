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

	void SetResource(Resource vb, Resource ib, UINT vertexCount, UINT indexCount) noexcept;
	void SetVertexHeapIndex(UINT heapIndex) noexcept { m_vertexHeapIndex = heapIndex; }
	void SetIndexHeapIndex(UINT heapIndex) noexcept { m_indexHeapIndex = heapIndex; }

	// VertexFormat::UI 등 CPU에서 재조립(월드 변환 후 배칭)이 필요한 mesh에 한해
	// 업로드 후에도 CPU 원본을 유지하기 위한 template. 대부분의 m_cpuTemplate는 null로 남는다.
	// (주의: "UI 포맷이면 유지"라는 가정에 의존 - 큰 UI mesh가 생기면 별도 플래그로 분리할 것)
	void SetCPUTemplate(std::shared_ptr<MeshAsset> asset) noexcept { m_cpuTemplate = std::move(asset); }
	const MeshAsset* GetCPUTemplate() const noexcept { return m_cpuTemplate.get(); }

private:
	Resource m_vb;
	Resource m_ib;

	UINT m_vertexCount{ 0 };
	UINT m_indexCount{ 0 };

	UINT m_vertexHeapIndex{ UINT_MAX };
	UINT m_indexHeapIndex{ UINT_MAX };

	std::shared_ptr<MeshAsset> m_cpuTemplate; // 대부분 null, template란, 무언가를 만들때 기준이 되는 원본
};

