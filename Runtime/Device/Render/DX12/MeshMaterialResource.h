#pragma once
#include "GameClient/Service/Render/Resource/IMaterialResource.h"
#include "GameClient/Service/Render/Desc/MeshMaterialDesc.h"
#include "Descriptor/DescriptorAllocation.h"

struct ITextureResource;

class MeshMaterialResource : public IMaterialResource
{
public:
	~MeshMaterialResource();
	MeshMaterialResource();
	virtual bool IsReady() const noexcept override;
	virtual MaterialType GetType() const noexcept override { return MaterialType::Mesh; }

	void SetTexture(MeshTextureSlot texSlot, std::shared_ptr<ITextureResource> texRes);
	void SetMaterialDesc(const MeshMaterialDesc& desc) { m_desc = desc; }
	const MeshMaterialDesc& GetMaterialDesc() const noexcept { return m_desc; }
	const MaterialSurface& GetSurface() const { return m_desc.surface; }
	const PipelineState& GetPipelineState() const { return m_desc.pipelineState; }

	DescriptorAllocation& GetTextureSRV(MeshTextureSlot texSlot);

private:
	std::vector<std::shared_ptr<ITextureResource>> m_texResources;
	MeshMaterialDesc m_desc;
};