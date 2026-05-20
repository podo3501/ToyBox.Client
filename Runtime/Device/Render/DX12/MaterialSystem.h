#pragma once
#include "GameClient/Service/Render/Repository/Material/IMaterialSystem.h"

class TextureSystem;

class MaterialSystem : public IMaterialSystem
{
public:
	~MaterialSystem();
	MaterialSystem(TextureSystem* texSystem);
	virtual shared_ptr<IMaterialResource> CreateMaterialResource(MaterialType materialType) override;
	virtual bool LoadFromAsset(
		std::shared_ptr<IMaterialResource> resource,
		std::shared_ptr<TextureAsset> texAsset,
		std::unique_ptr<MaterialDesc> matDesc) override;

	shared_ptr<IMaterialResource> GetDefaultMeshMaterial() { return m_defaultMeshMaterial; }

private:
	TextureSystem* m_texSystem{ nullptr };

	shared_ptr<IMaterialResource> m_defaultMeshMaterial;
};
