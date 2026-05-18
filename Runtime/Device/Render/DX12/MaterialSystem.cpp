#include "pch.h"
#include "MaterialSystem.h"
#include "MaterialResource.h"
#include "TextureSystem.h"
#include "TextureResource.h"

MaterialSystem::~MaterialSystem() = default;
MaterialSystem::MaterialSystem(TextureSystem* texSystem) :
	m_texSystem{ texSystem }
{
    m_defaultMaterial = CreateMaterialResource();
}

shared_ptr<IMaterialResource> MaterialSystem::CreateMaterialResource()
{
    auto materialRes = make_shared<MaterialResource>();
    materialRes->SetAlbedoTexture(m_texSystem->GetDefaultTexture()); //기본 텍스쳐를 셋팅한다.

    return materialRes;
}

bool MaterialSystem::LoadFromAsset(
    std::shared_ptr<IMaterialResource> resource, 
    std::shared_ptr<TextureAsset> albedoAsset,
    const MaterialDesc& matDesc)
{
    if (!resource)
        return false;

    auto matRes = std::static_pointer_cast<MaterialResource>(resource);
    auto texRes = m_texSystem->CreateTextureResource();
    if (!texRes)
        return false;

    if (!m_texSystem->LoadFromAsset(texRes, albedoAsset, matDesc.albedoDesc))
        return false;

    matRes->SetAlbedoTexture(texRes);
    matRes->SetMaterialDesc(matDesc);

    return true;
}