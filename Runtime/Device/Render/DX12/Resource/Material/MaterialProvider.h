#pragma once
#include "GameClient/Service/Render/Repository/Material/IMaterialProvider.h"

struct ID3D12Device;
class MaterialResource;
class TextureProvider;
enum class SurfaceType;

class MaterialProvider : public IMaterialProvider
{
public:
	~MaterialProvider();
	MaterialProvider(ID3D12Device* device, TextureProvider* texProvider);
	virtual shared_ptr<IMaterialResource> CreateMaterialResource(const MaterialDesc& matDesc) override;
	virtual bool LoadFromAsset(std::shared_ptr<IMaterialResource> resource, std::vector<std::shared_ptr<TextureAsset>> texAssets) override;

	void Update();
	std::shared_ptr<IMaterialResource> GetDefaultSurfaceMaterial(SurfaceType surfType);
	std::shared_ptr<IMaterialResource> GetDefaultDebugSurfMaterial();
	std::shared_ptr<IMaterialResource> GetDefaultUIMaterial();

private:
	template <typename DescType, typename ResourceType>
	void AddDefaultSurface();

	void SetDefaultTextures(MaterialResource* matRes);

	TextureProvider* m_texProvider{ nullptr };
	std::vector<std::shared_ptr<MaterialResource>> m_pendingMaterials;

	unordered_map<SurfaceType, shared_ptr<MaterialResource>> m_defaultSurfaceMaterials;
	shared_ptr<MaterialResource> m_defaultDebugSurfMats;
	shared_ptr<MaterialResource> m_defaultUIMaterial; //surface와 ui는 많이 다르기 때문에 따로 간다.
};
