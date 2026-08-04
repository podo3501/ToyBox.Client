#pragma once
#include "GameClient/Service/Render/Repository/Material/IMaterialProvider.h"
#include "../ResourceReleaseBuilder.h"

class MaterialResource;
class TextureProvider;

class MaterialProvider : public IMaterialProvider
{
public:
	~MaterialProvider();
	MaterialProvider(TextureProvider& texProvider, ResourceReleaseBuilder release) noexcept;

	virtual shared_ptr<IMaterialResource> CreateResource(const MaterialDesc& matDesc) override;
	virtual bool LoadResource(std::shared_ptr<IMaterialResource> resource,
		std::unordered_map<TextureSlot, std::shared_ptr<TextureAsset>> texAssets) override;
	virtual void ReleaseResource(std::shared_ptr<IMaterialResource> resource) override;
	void Update();

private:
	void SetDefaultTextures(MaterialResource* matRes);
	void FlushPendingMaterials();
	void FlushPendingRelease();

	TextureProvider& m_texProvider;
	ResourceReleaseBuilder m_releaseBuilder;
	std::vector<std::shared_ptr<MaterialResource>> m_pendingMaterials;
	std::vector<std::shared_ptr<IMaterialResource>> m_pendingReleases;
};
