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
	shared_ptr<IMaterialResource> GetDefaultMaterial(MaterialType type);

private:
	void SetDefaultTextures(MaterialResource* matRes, size_t slotCount);
	size_t GetTextureSlotCount(MaterialType type) const noexcept;

	unique_ptr<DescriptorFactory> m_descriptorFactory;
	TextureSystem* m_texSystem{ nullptr };

	std::unordered_map<MaterialType, std::shared_ptr<IMaterialResource>> m_defaultMaterials;
	std::vector<std::shared_ptr<MaterialResource>> m_pendingMaterials;
};
