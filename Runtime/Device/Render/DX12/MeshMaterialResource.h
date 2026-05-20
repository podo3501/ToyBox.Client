#pragma once
#include "GameClient/Service/Render/Resource/IMaterialResource.h"
#include "GameClient/Service/Render/Desc/MeshMaterialDesc.h"
#include "DescriptorAllocation.h"

struct ITextureResource;

class MeshMaterialResource : public IMaterialResource
{
public:
	~MeshMaterialResource();
	MeshMaterialResource();
	virtual bool IsReady() const noexcept override;

	void SetAlbedoTexture(std::shared_ptr<ITextureResource> texRes);
	void SetMaterialDesc(const MeshMaterialDesc& desc) { m_desc = desc; }
	const MaterialSurface& GetSurface() const { return m_desc.surface; }
	const PipelineState& GetPipelineState() const { return m_desc.pipelineState; }

	DescriptorAllocation& GetAlbedoTextureSRV();

private:
	shared_ptr<ITextureResource> m_texRes;
	MeshMaterialDesc m_desc;
};