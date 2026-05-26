#pragma once
#include "GameClient/Service/Render/Resource/IMaterialResource.h"
#include "GameClient/Service/Render/Desc/UIMaterialDesc.h"
#include "Descriptor/DescriptorAllocation.h"

struct ITextureResource;

class UIMaterialResource : public IMaterialResource
{
public:
	~UIMaterialResource();
	UIMaterialResource();
	virtual bool IsReady() const noexcept override;
	virtual MaterialType GetType() const noexcept override { return MaterialType::UI; }

	void SetTexture(std::shared_ptr<ITextureResource> texRes);
	void SetMaterialDesc(const UIMaterialDesc& desc) { m_desc = desc; }
	const UIMaterialDesc& GetMaterialDesc() const noexcept { return m_desc; }
	const PipelineState& GetPipelineState() const { return m_desc.pipelineState; }

	DescriptorAllocation& GetTextureSRV();

private:
	shared_ptr<ITextureResource> m_texRes;
	UIMaterialDesc m_desc;
};