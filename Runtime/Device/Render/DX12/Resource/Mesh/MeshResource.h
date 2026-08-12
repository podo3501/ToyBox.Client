#pragma once
#include "GameClient/Service/Render/Resource/IResource.h"

class MeshResource : public IResource
{
public:
	virtual ~MeshResource() override = default;

	virtual bool IsReady() const noexcept override { return m_ready; }
	virtual UINT GetVertexHeapIndex() const noexcept = 0;
	virtual UINT GetIndexHeapIndex() const noexcept = 0;
	virtual UINT GetVertexCount() const noexcept = 0;
	virtual UINT GetIndexCount() const noexcept = 0;

	void MarkReady() noexcept { m_ready = true; }

private:
	bool m_ready{ false };
};