#pragma once
#include "GameClient/Service/Render/Resource/IResource.h"
#include "GameClient/Service/Render/Definition/RenderState.h"
#include "GameClient/Asset/MaterialType.h"

class MaterialResource : public IResource
{
public:
	virtual ~MaterialResource() override = default;
	MaterialType GetType() const noexcept { return m_type; }
	const PipelineState& GetPipelineState() const noexcept { return m_pipelineState; }

protected:
	MaterialResource(MaterialType type, PipelineState pipelineState) :
		m_type{ type },
		m_pipelineState{ std::move(pipelineState) }
	{}

private:
	MaterialType m_type;
	const PipelineState m_pipelineState;
};