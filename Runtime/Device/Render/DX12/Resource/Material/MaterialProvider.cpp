#include "pch.h"
#include "MaterialProvider.h"
#include "PhongMaterialResource.h"
#include "PbrMaterialResource.h"
#include "GridMaterialResource.h"
#include "UIMaterialResource.h"
#include "../Texture/TextureProvider.h"
#include "../Texture/TextureResource.h"

template <typename DescType, typename ResourceType>
void MaterialProvider::AddDefaultSurface()
{
    DescType desc{};
    auto defaultMat = std::make_shared<ResourceType>(desc);
    SetDefaultTextures(defaultMat.get());
    m_defaultSurfaceMaterials[desc.surfType] = defaultMat;
}

MaterialProvider::~MaterialProvider() = default;
MaterialProvider::MaterialProvider(TextureProvider* texProvider) :
	m_texProvider{ texProvider }
{
    AddDefaultSurface<PhongMaterialDesc, PhongMaterialResource>();
    AddDefaultSurface<PbrMaterialDesc, PbrMaterialResource>();

    GridMaterialDesc gridDesc;
    m_defaultDebugSurfMats = make_shared<GridMaterialResource>(gridDesc);
    SetDefaultTextures(m_defaultDebugSurfMats.get());

    UIMaterialDesc uiDesc;
    m_defaultUIMaterial = make_shared<UIMaterialResource>(uiDesc);
    SetDefaultTextures(m_defaultUIMaterial.get());
}

static std::shared_ptr<MaterialResource> CreateSurfaceResource(const SurfaceMaterialDesc& surfaceDesc)
{
    switch (surfaceDesc.surfType)
    {
    case SurfaceType::Phong: return std::make_shared<PhongMaterialResource>(surfaceDesc);
    case SurfaceType::PBR: return std::make_shared<PbrMaterialResource>(surfaceDesc);
    }

    return nullptr;
}

shared_ptr<IMaterialResource> MaterialProvider::CreateMaterialResource(const MaterialDesc& matDesc)
{
    shared_ptr<MaterialResource> matRes{ nullptr };

    switch (matDesc.domain)
    {
    case MaterialDomain::Surface:
        matRes = CreateSurfaceResource(static_cast<const SurfaceMaterialDesc&>(matDesc));
        break;
    case MaterialDomain::DebugSurface:
        matRes = std::make_shared<GridMaterialResource>(matDesc);
        break;
    case MaterialDomain::UserInterface:
        matRes = make_shared<UIMaterialResource>(matDesc);
        break;
    }

    if (!matRes)
        return nullptr;

    SetDefaultTextures(matRes.get());

    return matRes;
}

void MaterialProvider::SetDefaultTextures(MaterialResource* matRes)
{
    if (!matRes) return;

    auto defaultTypes = matRes->GetRequiredDefaultTextures();
    for (size_t i = 0; i < defaultTypes.size(); ++i)
    {
        auto defaultTex = m_texProvider->GetDefaultTexture(defaultTypes[i]);
        matRes->SetTexture(static_cast<TextureSlot>(i), defaultTex);
    }
}

bool MaterialProvider::LoadFromAsset(
    std::shared_ptr<IMaterialResource> res,
    std::vector<std::shared_ptr<TextureAsset>> texAssets)
{
    auto matRes = static_pointer_cast<MaterialResource>(res);
    if (!matRes)
        return false;

    auto& matDesc = matRes->GetMaterialDesc();
    for (size_t i = 0; i < texAssets.size(); ++i)
    {
        auto& texAsset = texAssets[i];
        if (!texAsset)
            continue;

        auto& texDesc = matDesc.textures[i];
        auto texRes = m_texProvider->CreateTextureResource(texDesc);
        if (!texRes)
            return false;

        if (!m_texProvider->LoadFromAsset(texRes, texAsset))
            return false;

        matRes->SetTexture(static_cast<TextureSlot>(i), texRes);
    }
    m_pendingMaterials.push_back(matRes);

    return true;
}

void MaterialProvider::Update()
{
    for (auto it = m_pendingMaterials.begin(); it != m_pendingMaterials.end();)
    {
        auto& matRes = *it;
        if(!matRes->IsTextureReady())
        {
            ++it;
            continue;
        }

        matRes->MarkReady();
        it = m_pendingMaterials.erase(it);
    }
}

std::shared_ptr<IMaterialResource> MaterialProvider::GetDefaultSurfaceMaterial(SurfaceType surfType)
{
    auto it = m_defaultSurfaceMaterials.find(surfType);
    if (it != m_defaultSurfaceMaterials.end())
    {
        return it->second;
    }
    return nullptr;
}

shared_ptr<IMaterialResource> MaterialProvider::GetDefaultDebugSurfMaterial()
{
    return m_defaultDebugSurfMats;
}

shared_ptr<IMaterialResource> MaterialProvider::GetDefaultUIMaterial()
{
    return m_defaultUIMaterial;
}
