#pragma once
#include "MaterialResource.h"
#include "GameClient/Service/Render/Desc/UIMaterialDesc.h"

struct ITextureResource;

class UIMaterialResource : public MaterialResource
{
public:
	~UIMaterialResource();
	UIMaterialResource() = delete;
	UIMaterialResource(const MaterialDesc& desc);

	virtual std::vector<DefaultTextureType> GetRequiredDefaultTextures() const override;
	virtual const MaterialDesc& GetMaterialDesc() const noexcept override { return m_desc; }

private:
	UIMaterialDesc m_desc;
};