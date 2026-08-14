#pragma once
#include "GameClient/Service/Render/Resource/IResource.h"
#include "GameClient/Service/Render/Definition/RenderState.h"

class DebugMaterialResource : public IResource
{
public:
	virtual ~DebugMaterialResource() override = default;
	const PipelineState& GetPipelineState() const noexcept { return m_pipelineState; }

protected:
	explicit DebugMaterialResource(PipelineState pipelineState)
		: m_pipelineState{ std::move(pipelineState) }
	{}

private:
	const PipelineState m_pipelineState;
};