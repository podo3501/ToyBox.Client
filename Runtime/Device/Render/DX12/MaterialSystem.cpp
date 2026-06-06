#include "pch.h"
#include "MaterialSystem.h"
#include "MaterialResource/PbrMaterialResource.h"
#include "MaterialResource/GridMaterialResource.h"
#include "Descriptor/DescriptorFactory.h"
#include "MaterialResource/UIMaterialResource.h"
#include "TextureSystem.h"
#include "TextureResource.h"

MaterialSystem::~MaterialSystem() = default;
MaterialSystem::MaterialSystem(ID3D12Device* device, DescriptorAllocator* srvAllocator, TextureSystem* texSystem) :
    m_descriptorFactory{ make_unique<DescriptorFactory>(device, srvAllocator) },
	m_texSystem{ texSystem }
{
    PbrMaterialDesc pbrDesc;
    m_defaultPbrMaterial = make_shared<PbrMaterialResource>(pbrDesc);
    SetDefaultTextures(m_defaultPbrMaterial.get());

    GridMaterialDesc gridDesc;
    m_defaultGridMaterial = make_shared<GridMaterialResource>(gridDesc);
    SetDefaultTextures(m_defaultGridMaterial.get());

    UIMaterialDesc uiDesc;
    m_defaultUIMaterial = make_shared<UIMaterialResource>(uiDesc);
    SetDefaultTextures(m_defaultUIMaterial.get());
}

shared_ptr<IMaterialResource> MaterialSystem::CreateMaterialResource(const MaterialDesc& matDesc)
{
    shared_ptr<MaterialResource> matRes{ nullptr };

    switch (matDesc.domain)
    {
    case MaterialDomain::Surface:
    {
        const auto& meshDesc = static_cast<const SurfaceMaterialDesc&>(matDesc);

        if (meshDesc.surfType == SurfaceType::PBR) 
            matRes = make_shared<PbrMaterialResource>(matDesc);
        else if (meshDesc.surfType == SurfaceType::Grid)
            matRes = make_shared<GridMaterialResource>(matDesc);
        break;
    }
    case MaterialDomain::UserInterface:
        matRes = make_shared<UIMaterialResource>(matDesc);
        break;
    }

    if (!matRes)
        return nullptr;

    SetDefaultTextures(matRes.get());

    return matRes;
}

void MaterialSystem::SetDefaultTextures(MaterialResource* matRes)
{
    if (!matRes) return;

    auto defaultTypes = matRes->GetRequiredDefaultTextures();
    for (size_t i = 0; i < defaultTypes.size(); ++i)
    {
        auto defaultTex = m_texSystem->GetDefaultTexture(defaultTypes[i]);
        matRes->SetTexture(static_cast<TextureSlot>(i), defaultTex);
    }
}

bool MaterialSystem::LoadFromAsset(
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
        auto texRes = m_texSystem->CreateTextureResource(texDesc);
        if (!texRes)
            return false;

        if (!m_texSystem->LoadFromAsset(texRes, texAsset))
            return false;

        matRes->SetTexture(static_cast<TextureSlot>(i), texRes);
    }
    m_pendingMaterials.push_back(matRes);

    return true;
}

void MaterialSystem::Update()
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

shared_ptr<IMaterialResource> MaterialSystem::GetDefaultPbrMaterial() { return m_defaultPbrMaterial; }
shared_ptr<IMaterialResource> MaterialSystem::GetDefaultGridMaterial() { return m_defaultGridMaterial; }
shared_ptr<IMaterialResource> MaterialSystem::GetDefaultUIMaterial() { return m_defaultUIMaterial; }
