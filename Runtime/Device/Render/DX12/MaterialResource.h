#pragma once
#include "GameClient/Service/Render/Repository/IMaterialResource.h"
#include "GameClient/Service/Render/Repository/MaterialDesc.h"
#include "DescriptorAllocation.h"

struct ITextureResource;

class MaterialResource : public IMaterialResource
{
public:
	~MaterialResource();
	MaterialResource();
	virtual bool IsReady() const noexcept override;

	void SetAlbedoTexture(std::shared_ptr<ITextureResource> texRes);
	void SetMaterialDesc(const MaterialDesc& desc) { m_desc = desc; }
	const MaterialSurface& GetSurface() const { return m_desc.surface; }
	const PipelineState& GetPipelineState() const { return m_desc.pipelineState; }

	DescriptorAllocation& GetAlbedoTextureSRV();

private:
	shared_ptr<ITextureResource> m_texRes;
	MaterialDesc m_desc;
};