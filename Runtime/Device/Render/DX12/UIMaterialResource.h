#pragma once
#include "MaterialResource.h"
#include "GameClient/Service/Render/Desc/UIMaterialDesc.h"

struct ITextureResource;

class UIMaterialResource : public MaterialResource
{
public:
	~UIMaterialResource();
	UIMaterialResource();
	virtual bool IsReady() const noexcept override { return m_ready; }

	virtual void MarkReady() noexcept override { m_ready = true; }
	virtual bool IsTextureReady() const noexcept override;
	virtual MaterialType GetType() const noexcept override { return MaterialType::UI; }
	virtual void SetMaterialDesc(const MaterialDesc& desc) noexcept override { m_desc = static_cast<const UIMaterialDesc&>(desc); }
	virtual const MaterialDesc& GetMaterialDesc() const noexcept override { return m_desc; }
	virtual void SetTexture(TextureSlot texSlot, std::shared_ptr<ITextureResource> texRes) noexcept override;
	virtual std::vector<std::shared_ptr<ITextureResource>> GetTextures() const noexcept override { return { m_texResource }; }

	UINT GetTextureHeapIndex() const noexcept;
	const PipelineState& GetPipelineState() const { return m_desc.pipelineState; }

private:
	shared_ptr<ITextureResource> m_texResource;
	UIMaterialDesc m_desc;

	bool m_ready{ false };
};