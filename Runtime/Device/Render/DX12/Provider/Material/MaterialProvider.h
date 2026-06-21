#pragma once
#include "GameClient/Service/Render/Repository/Material/IMaterialProvider.h"

class MaterialResource;
class TextureProvider;

class MaterialProvider : public IMaterialProvider
{
public:
	~MaterialProvider();
	explicit MaterialProvider(TextureProvider& texProvider);
	virtual shared_ptr<IMaterialResource> CreateMaterialResource(const MaterialDesc& matDesc) override;
	virtual bool LoadFromAsset(std::shared_ptr<IMaterialResource> resource, 
		std::unordered_map<TextureSlot, std::shared_ptr<TextureAsset>> texAssets) override;
	void Update();

private:
	void SetDefaultTextures(MaterialResource* matRes);

	TextureProvider& m_texProvider;
	std::vector<std::shared_ptr<MaterialResource>> m_pendingMaterials;
};
