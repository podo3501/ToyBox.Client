#include "pch.h"
#include "EnvironmentProvider.h"
#include "../PendingLoadQueue.h"
#include "../PendingReleaseQueue.h"
#include "../Texture/TextureCubeProvider.h"
#include "Resource/Environment/EnvironmentResource.h"
#include "GameClient/Asset/EnvironmentAsset.h"
#include "Core/Foundation/Cast.hpp"

EnvironmentProvider::~EnvironmentProvider() = default;
EnvironmentProvider::EnvironmentProvider(
    PendingLoadQueue& pendingLoad,
    PendingReleaseQueue& pendingRelease, 
    TextureCubeProvider& cubeProvider) noexcept :
    m_pendingLoad{ pendingLoad },
    m_pendingRelease{ pendingRelease },
    m_cubeProvider{ cubeProvider }
{}

std::shared_ptr<IResource> EnvironmentProvider::CreateResource(
    std::shared_ptr<AssetData> asset)
{
    if (!asset)
        return nullptr;

    auto envAsset = Core::Cast<EnvironmentAsset>(asset);
    if (!envAsset->skybox || !envAsset->reflection || !envAsset->irradiance)
        return nullptr;

    auto skyboxRes = CreateCubeResource(envAsset->skybox);
    if (!skyboxRes)
        return nullptr;

    auto reflectionRes = CreateCubeResource(envAsset->reflection);
    if (!reflectionRes)
        return nullptr;

    auto envRes = std::make_shared<EnvironmentResource>();
    envRes->SetSkybox(skyboxRes);
    envRes->SetReflection(reflectionRes);
    envRes->SetIrradianceSH(envAsset->irradiance->coefficients);

    m_pendingLoad.Add(envRes);
    return envRes;
}

std::shared_ptr<TextureCubeResource> EnvironmentProvider::CreateCubeResource(
    std::shared_ptr<TextureCubeAsset> cubeAsset)
{
    auto res = m_cubeProvider.CreateResource();
    if (!res)
        return nullptr;

    if (!m_cubeProvider.LoadResource(res, cubeAsset))
        return nullptr;

    return res;
}

void EnvironmentProvider::ReleaseResource(std::shared_ptr<IResource> res)
{
    m_pendingRelease.Add(std::move(res));
}