#include "pch.h"
#include "MaterialSystem.h"
#include "MeshMaterialResource.h"
#include "UIMaterialResource.h"
#include "TextureSystem.h"
#include "TextureResource.h"

MaterialSystem::~MaterialSystem() = default;
MaterialSystem::MaterialSystem(TextureSystem* texSystem) :
	m_texSystem{ texSystem }
{
    m_defaultMeshMaterial = CreateMaterialResource(MaterialType::Mesh);
}

shared_ptr<IMaterialResource> MaterialSystem::CreateMaterialResource(MaterialType materialType)
{
    switch (materialType)
    {
    case MaterialType::Mesh:
    {
        auto res = make_shared<MeshMaterialResource>();
        res->SetAlbedoTexture(m_texSystem->GetDefaultTexture());
        return res;
    }

    case MaterialType::UI:
    {
        auto res = make_shared<UIMaterialResource>();
        res->SetTexture(m_texSystem->GetDefaultTexture());
        return res;
    }
    }

    return nullptr;
}

bool MaterialSystem::LoadFromAsset(
    std::shared_ptr<IMaterialResource> resource, 
    std::shared_ptr<TextureAsset> texAsset,
    std::unique_ptr<MaterialDesc> matDesc)
{
    if (!resource || !matDesc)
        return false;

    switch (matDesc->type)
    {
    case MaterialType::Mesh:
    {
        auto meshRes = std::static_pointer_cast<MeshMaterialResource>(resource);
        if (!meshRes)
            return false;

        auto* meshDesc = static_cast<MeshMaterialDesc*>(matDesc.get());
        if (!meshDesc)
            return false;

        auto texRes = m_texSystem->CreateTextureResource();
        if (!texRes)
            return false;

        if (!m_texSystem->LoadFromAsset(texRes, texAsset, meshDesc->albedoDesc))
            return false;

        meshRes->SetAlbedoTexture(texRes);
        meshRes->SetMaterialDesc(*meshDesc);

        return true;
    }

    case MaterialType::UI:
    {
        auto uiRes = std::static_pointer_cast<UIMaterialResource>(resource);
        if (!uiRes)
            return false;

        auto* uiDesc = static_cast<UIMaterialDesc*>(matDesc.get());
        if (!uiDesc)
            return false;

        auto texRes = m_texSystem->CreateTextureResource();
        if (!texRes)
            return false;

        if (!m_texSystem->LoadFromAsset(texRes, texAsset, uiDesc->texDesc))
            return false;

        uiRes->SetTexture(texRes);
        uiRes->SetMaterialDesc(*uiDesc);

        return true;
    }
    }

    return false;
}