#pragma once
#include "GameClient/Service/Render/Repository/Material/IMaterialSystem.h"

struct ID3D12Device;
struct MaterialResource;
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
	shared_ptr<IMaterialResource> GetDefaultMeshMaterial();

private:
	void FinalizeMaterial(const std::shared_ptr<MaterialResource>& material);

	unique_ptr<DescriptorFactory> m_descriptorFactory;
	TextureSystem* m_texSystem{ nullptr };

	shared_ptr<MaterialResource> m_defaultMeshMaterial;
	std::vector<std::shared_ptr<MaterialResource>> m_pendingMaterials;
};
