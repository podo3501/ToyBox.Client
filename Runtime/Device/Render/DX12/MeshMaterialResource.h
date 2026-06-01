#pragma once
#include "MaterialResource.h"
#include "GameClient/Service/Render/Desc/MeshMaterialDesc.h"

struct ITextureResource;

class MeshMaterialResource : public MaterialResource
{
public:
	~MeshMaterialResource();
	MeshMaterialResource();
	virtual bool IsReady() const noexcept override { return m_ready; }

	virtual void MarkReady() noexcept override { m_ready = true; }
	virtual bool IsTextureReady() const noexcept override;
	virtual MaterialType GetType() const noexcept override { return MaterialType::Mesh; }
	virtual void SetMaterialDesc(const MaterialDesc& desc) noexcept override { m_desc = static_cast<const MeshMaterialDesc&>(desc); }
	virtual const MaterialDesc& GetMaterialDesc() const noexcept override { return m_desc; }
	virtual void SetTexture(TextureSlot texSlot, std::shared_ptr<ITextureResource> texRes) noexcept override;
	virtual std::vector<std::shared_ptr<ITextureResource>> GetTextures() const noexcept;
	virtual std::vector<UINT> GetTextureIndices() const noexcept;
	
	const MaterialSurface& GetSurface() const { return m_desc.surface; }
	const PipelineState& GetPipelineState() const { return m_desc.pipelineState; }

private:
	std::vector<std::shared_ptr<ITextureResource>> m_texResources;
	MeshMaterialDesc m_desc;

	bool m_ready{ false };
};