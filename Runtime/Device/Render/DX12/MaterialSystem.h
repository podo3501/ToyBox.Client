#pragma once
#include "GameClient/Service/Render/Repository/Material/IMaterialSystem.h"

struct ID3D12Device;
class MaterialResource;
class TextureSystem;
class DescriptorAllocator;
class DescriptorFactory;

class MaterialSystem : public IMaterialSystem
{
public:
	~MaterialSystem();
	MaterialSystem(ID3D12Device* device, DescriptorAllocator* srvAllocator, TextureSystem* texSystem);
	virtual shared_ptr<IMaterialResource> CreateMaterialResource(const MaterialDesc& matDesc) override;
	virtual bool LoadFromAsset(std::shared_ptr<IMaterialResource> resource, std::vector<std::shared_ptr<TextureAsset>> texAssets) override;

	void Update();
	shared_ptr<IMaterialResource> GetDefaultPbrMaterial();
	shared_ptr<IMaterialResource> GetDefaultGridMaterial();
	shared_ptr<IMaterialResource> GetDefaultUIMaterial();

private:
	void SetDefaultTextures(MaterialResource* matRes);

	unique_ptr<DescriptorFactory> m_descriptorFactory;
	TextureSystem* m_texSystem{ nullptr };
	std::vector<std::shared_ptr<MaterialResource>> m_pendingMaterials;

	shared_ptr<MaterialResource> m_defaultPbrMaterial;
	shared_ptr<MaterialResource> m_defaultGridMaterial;
	shared_ptr<MaterialResource> m_defaultUIMaterial;
};
