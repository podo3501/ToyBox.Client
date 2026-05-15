#pragma once
#include "GameClient/Service/Render/Repository/IMaterialResource.h"
#include "DescriptorAllocation.h"

struct ITextureResource;

class MaterialResource : public IMaterialResource
{
public:
	~MaterialResource();
	MaterialResource();
	virtual bool IsReady() const noexcept override;

	void SetTexture(std::shared_ptr<ITextureResource> texRes);
	DescriptorAllocation& GetTextureSRV();

private:
	shared_ptr<ITextureResource> m_texRes;
};