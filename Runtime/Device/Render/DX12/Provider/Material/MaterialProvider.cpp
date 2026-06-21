#include "pch.h"
#include "MaterialProvider.h"
#include "Resource/Material/PhongMaterialResource.h"
#include "Resource/Material/PbrMaterialResource.h"
#include "Resource/Material/GridMaterialResource.h"
#include "Resource/Material/UIMaterialResource.h"
#include "Resource/Texture/TextureResource.h"
#include "../Texture/TextureProvider.h"


MaterialProvider::~MaterialProvider() = default;
MaterialProvider::MaterialProvider(TextureProvider& texProvider) :
	m_texProvider{ texProvider }
{}

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

    auto defaultBindings = matRes->GetDefaultTextureBindings();
    for (auto& binding : defaultBindings)
    {
        auto tex = m_texProvider.GetDefaultTexture(binding.type);
        matRes->SetTexture(binding.slot, tex);
    }
}

bool MaterialProvider::LoadFromAsset(
    std::shared_ptr<IMaterialResource> res,
    std::unordered_map<TextureSlot, std::shared_ptr<TextureAsset>> texAssets)
{
    auto matRes = static_pointer_cast<MaterialResource>(res);
    if (!matRes)
        return false;

    auto& matDesc = matRes->GetMaterialDesc();
    for (const auto& [slot, texDesc] : matDesc.textures)
    {
        auto it = texAssets.find(slot);
        if (it == texAssets.end() || !it->second)
            continue;

        auto texRes = m_texProvider.CreateTextureResource(texDesc);
        if (!texRes)
            return false;

        if (!m_texProvider.LoadFromAsset(texRes, it->second))
            return false;

        matRes->SetTexture(slot, texRes);
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
