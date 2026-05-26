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
    auto res = make_shared<MeshMaterialResource>();
    res->SetTexture(TextureSlot::Albedo, m_texSystem->GetDefaultTexture());

    m_defaultMeshMaterial = res;
}

shared_ptr<IMaterialResource> MaterialSystem::CreateMaterialResource(const MaterialDesc& matDesc)
{
    switch (matDesc.type)
    {
    case MaterialType::Mesh:
    {
        auto res = make_shared<MeshMaterialResource>();
        res->SetTexture(TextureSlot::Albedo, m_texSystem->GetDefaultTexture());
        res->SetMaterialDesc(static_cast<const MeshMaterialDesc&>(matDesc));
        return res;
    }

    case MaterialType::UI:
    {
        auto res = make_shared<UIMaterialResource>();
        res->SetTexture(m_texSystem->GetDefaultTexture());
        res->SetMaterialDesc(static_cast<const UIMaterialDesc&>(matDesc));
        return res;
    }
    }

    return nullptr;
}

//일단 한장으로 되게끔 하고 나중에 여러장 되게끔 하자.
        //Asset에서 update 에서 읽어야할 파일을 읽어서 로딩하는 로직 안짰음.
        //MaterialRepository에서 코딩 안 짰음.
        //MaterialResource에 desc를 넣은 다음 읽을때 꺼내서 읽는 방식으로 하고 있음.
        //여러장 로딩할때 texAsset인자에 타입값과 같이 보내서 알베도인지, normal인지 구분하는 방식으로 가는게 좋겠음.

bool MaterialSystem::LoadFromAsset(
    std::shared_ptr<IMaterialResource> resource,
    TextureSlot texSlot,
    std::shared_ptr<TextureAsset> texAsset)
{
    if (!resource)
        return false;

    switch (resource->GetType())
    {
    case MaterialType::Mesh:
    {
        auto meshRes = std::static_pointer_cast<MeshMaterialResource>(resource);
        if (!meshRes)
            return false;

        auto& matDesc = meshRes->GetMaterialDesc();
        auto texRes = m_texSystem->CreateTextureResource();
        if (!texRes)
            return false;

        auto& slotDesc = matDesc.textures[static_cast<size_t>(texSlot)].desc;
        if (!m_texSystem->LoadFromAsset(texRes, texAsset, slotDesc))
            return false;

        meshRes->SetTexture(texSlot, texRes);
        return true;
    }

    case MaterialType::UI:
    {
        auto uiRes = std::static_pointer_cast<UIMaterialResource>(resource);
        if (!uiRes)
            return false;

        auto& uiDesc = uiRes->GetMaterialDesc();
        auto texRes = m_texSystem->CreateTextureResource();
        if (!texRes)
            return false;

        auto& slotDesc = uiDesc.textures[static_cast<size_t>(texSlot)].desc;
        if (!m_texSystem->LoadFromAsset(texRes, texAsset, slotDesc))
            return false;

        uiRes->SetTexture(texRes);
        return true;
    }
    }

    return false;
}