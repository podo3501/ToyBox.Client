#pragma once
#include "GameClient/Service/Render/Repository/IMaterialSystem.h"

class TextureSystem;

class MaterialSystem : public IMaterialSystem
{
public:
	~MaterialSystem();
	MaterialSystem(TextureSystem* texSystem);
	virtual shared_ptr<IMaterialResource> CreateMaterialResource() override;
	virtual bool LoadFromAsset(
		std::shared_ptr<IMaterialResource> resource, 
		std::shared_ptr<TextureAsset> albedoAsset,
		const MaterialDesc& matDesc) override;

	shared_ptr<IMaterialResource> GetDefaultMaterial() { return m_defaultMaterial; }

private:
	TextureSystem* m_texSystem{ nullptr };

	shared_ptr<IMaterialResource> m_defaultMaterial;
};
