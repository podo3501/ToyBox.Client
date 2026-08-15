#include "pch.h"
#include "MaterialProv.h"
#include "Resource/Material/PhongMaterialRes.h"
#include "Resource/Material/PbrMaterialRes.h"
#include "Resource/Material/GridMaterialResource.h"
#include "Resource/Texture/TextureResource.h"
#include "../Texture/TextureProvider.h"

MaterialProv::~MaterialProv() = default;
MaterialProv::MaterialProv(TextureProvider& texProvider, ResourceReleaseBuilder release) noexcept :
	m_texProvider{ texProvider },
    m_releaseBuilder{ std::move(release) }
{}

static std::shared_ptr<MaterialRes> CreateSurfaceResource(const SurfaceMaterialDesc& surfaceDesc)
{
    switch (surfaceDesc.surfType)
    {
    case SurfaceType::Phong: return std::make_shared<PhongMaterialRes>(surfaceDesc);
    case SurfaceType::PBR: return std::make_shared<PbrMaterialRes>(surfaceDesc);
    }

    return nullptr;
}

shared_ptr<IMaterialResource> MaterialProv::CreateResource(const MaterialDesc& matDesc)
{
    shared_ptr<MaterialRes> matRes{ nullptr };

    switch (matDesc.domain)
    {
    case MaterialDomain::Surface:
        matRes = CreateSurfaceResource(static_cast<const SurfaceMaterialDesc&>(matDesc));
        break;
    case MaterialDomain::DebugSurface:
        matRes = std::make_shared<GridMaterialResource>(matDesc);
        break;
    }

    if (!matRes)
        return nullptr;

    SetDefaultTextures(matRes.get());

    return matRes;
}

void MaterialProv::SetDefaultTextures(MaterialRes* matRes)
{
    if (!matRes) return;

    auto defaultBindings = matRes->GetBuiltinTextureBindings();
    for (auto& binding : defaultBindings)
    {
        auto tex = m_texProvider.GetBuiltinTexture(binding.type);
        matRes->SetTexture(binding.slot, tex);
    }
}

bool MaterialProv::LoadResource(
    std::shared_ptr<IMaterialResource> res,
    std::unordered_map<TextureSlot, std::shared_ptr<TextureAsset>> texAssets)
{
    auto matRes = static_pointer_cast<MaterialRes>(res);
    if (!matRes)
        return false;

    auto& matDesc = matRes->GetMaterialDesc();
    for (const auto& bindTex : matDesc.textures)
    {
        auto it = texAssets.find(bindTex.slot);
        if (it == texAssets.end() || !it->second)
            continue;

        auto texRes = m_texProvider.CreateResource();
        if (!texRes)
            return false;

        if (!m_texProvider.LoadResource(texRes, it->second))
            return false;

        matRes->SetTexture(bindTex.slot, texRes);
    }

    m_pendingMaterials.push_back(matRes);
    return true;
}

void MaterialProv::ReleaseResource(std::shared_ptr<IMaterialResource> resource)
{
    if (!resource)
        return;

    m_pendingReleases.emplace_back(std::move(resource));
}

void MaterialProv::Update()
{
    FlushPendingMaterials();
    FlushPendingRelease();
}

void MaterialProv::FlushPendingMaterials()
{
    for (auto it = m_pendingMaterials.begin(); it != m_pendingMaterials.end();)
    {
        auto& matRes = *it;
        if (!matRes->IsTextureReady())
        {
            ++it;
            continue;
        }

        matRes->MarkReady();
        it = m_pendingMaterials.erase(it);
    }
}

void MaterialProv::FlushPendingRelease()
{
    if (m_pendingReleases.empty())
        return;

    m_releaseBuilder.ReleaseResources(std::move(m_pendingReleases));
}