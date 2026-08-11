#include "pch.h"
#include "EnvironmentProvider.h"
#include "Resource/Environment/EnvironmentResource.h"
#include "../Texture/TextureCubeProvider.h"
#include "GameClient/Asset/EnvironmentAsset.h"

EnvironmentProvider::~EnvironmentProvider() = default;
EnvironmentProvider::EnvironmentProvider(TaskScheduler& taskScheduler, TextureCubeProvider& cubeProvider) noexcept :
    ResourceProvider{ taskScheduler },
    m_cubeProvider{ cubeProvider }
{}

std::shared_ptr<IResource> EnvironmentProvider::CreateResource(
    std::shared_ptr<AssetData> asset)
{
    if (!asset)
        return nullptr;

    auto envAsset = std::static_pointer_cast<EnvironmentAsset>(asset);
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

    m_pendingLoads.push_back(envRes);
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

void EnvironmentProvider::Update()
{
    FlushPendingLoad();
    FlushPendingRelease();
}