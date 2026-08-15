#include "pch.h"
#include "MaterialProvider.h"
#include "GameClient/Asset/PbrMaterialAsset.h"
#include "GameClient/Asset/PhongMaterialAsset.h"
#include "Resource/Material/PbrMaterialResource.h"
#include "Resource/Material/PhongMaterialResource.h"
#include "Core/Foundation/Cast.hpp"
#include "../Texture/TextureProvider.h"

MaterialProvider::~MaterialProvider() = default;
MaterialProvider::MaterialProvider(TaskScheduler& taskScheduler, TextureProvider& texProvider) noexcept :
    m_pendingRelease{ taskScheduler },
    m_texProvider{ texProvider }
{}

std::shared_ptr<IResource> MaterialProvider::CreateResource(std::shared_ptr<AssetData> asset)
{
    if (!asset)
        return nullptr;

    auto matAsset = Core::Cast<MaterialAsset>(asset);
    if (!matAsset)
        return nullptr;

    shared_ptr<IResource> matRes{ nullptr };

    switch (matAsset->type)
    {
    case MaterialType::PBR: matRes = CreatePbrMaterialResource(asset); break;
    case MaterialType::Phong: matRes = CreatePhongMaterialResource(asset); break;
    }
    Assert(matRes);

    return matRes;
}

shared_ptr<IResource> MaterialProvider::CreatePbrMaterialResource(std::shared_ptr<AssetData> asset)
{
    auto pbrAsset = Core::Cast<PbrMaterialAsset>(asset);

    auto albedoRes = CreateTexResource(pbrAsset->albedo);
    if (!albedoRes)
        return nullptr;

    auto normalRes = CreateTexResource(pbrAsset->normal);
    auto armRes = CreateTexResource(pbrAsset->arm);

    auto pbrRes = std::make_shared<PbrMaterialResource>();
    pbrRes->SetAlbedo(albedoRes);
    pbrRes->SetNormal(normalRes);
    pbrRes->SetArm(armRes);
    pbrRes->SetSurface(pbrAsset->surface);

    m_pendingLoad.Add(pbrRes);
    return pbrRes;
}

shared_ptr<IResource> MaterialProvider::CreatePhongMaterialResource(std::shared_ptr<AssetData> asset)
{
    auto phongAsset = Core::Cast<PhongMaterialAsset>(asset);

    auto albedoRes = CreateTexResource(phongAsset->albedo);
    if (!albedoRes)
        return nullptr;

    auto normalRes = CreateTexResource(phongAsset->normal);

    auto phongRes = std::make_shared<PhongMaterialResource>();
    phongRes->SetAlbedo(albedoRes);
    phongRes->SetNormal(normalRes);
    phongRes->SetSurface(phongAsset->surface);

    m_pendingLoad.Add(phongRes);
    return phongRes;
}

std::shared_ptr<TextureResource> MaterialProvider::CreateTexResource(std::shared_ptr<TextureAsset> texAsset)
{
    if (!texAsset) return nullptr;

    auto res = m_texProvider.CreateResource();
    if (!res) return nullptr;

    if (!m_texProvider.LoadResource(res, texAsset))
        return nullptr;

    return res;
}

void MaterialProvider::ReleaseResource(std::shared_ptr<IResource> res)
{
    m_pendingRelease.Add(std::move(res));
}

void MaterialProvider::Update()
{
    m_pendingLoad.Flush();
    m_pendingRelease.Flush();
}