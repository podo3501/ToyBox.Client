#pragma once
#include "GameClient/Service/Render/Resource/IMaterialResource.h"
#include "Resource/Texture/DefaultTextureType.h"

class TextureResource;

class MaterialResource : public IMaterialResource //backend용 material resource 인터페이스.
{
public:
	virtual ~MaterialResource() override;
	MaterialResource() = delete;
	MaterialResource(uint32_t texSlotCount);
	virtual bool IsReady() const noexcept override { return m_ready; }

	//backend interface
	virtual const MaterialDesc& GetMaterialDesc() const noexcept = 0;
	virtual std::vector<DefaultTextureBinding> GetDefaultTextureBindings() const { return {}; }

	void MarkReady() noexcept { m_ready = true; }
	const PipelineState& GetPipelineState() const;
	MaterialDomain GetDomain() const noexcept;
	bool IsTextureReady() const noexcept;
	void SetTexture(TextureSlot texSlot, std::shared_ptr<TextureResource> texRes) noexcept;
	TextureResource* GetTexture(TextureSlot texSlot) const noexcept;
	std::vector<UINT> GetTextureIndices() const noexcept;

private:
	bool m_ready{ false };
	std::vector<std::shared_ptr<TextureResource>> m_texResources;
};