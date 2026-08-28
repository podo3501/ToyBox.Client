#pragma once
#include "GameClient/Service/Render/Resource/IResource.h"
#include "Definition/RenderState.h"
#include "GameClient/Asset/MaterialType.h"

class MaterialResource : public IResource
{
public:
	virtual ~MaterialResource() override;
	MaterialResource() = delete;
	MaterialType GetType() const noexcept { return m_type; }

	PipelineState GetPipelineState(
		const std::optional<RasterPreset>& rasterOverride,
		const std::optional<ShaderID>& shaderOverride) const;
	const PipelineState& GetPipelineState() const { return m_pipelineState; }

protected:
	MaterialResource(MaterialType type, PipelineState pipelineState) :
		m_type{ type },
		m_pipelineState{ std::move(pipelineState) }
	{}

private:
	MaterialType m_type;
	PipelineState m_pipelineState;
};