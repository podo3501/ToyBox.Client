#include "pch.h"
#include "EnvironmentProvider.h"
#include "Resource/Environment/EnvironmentResource.h"
#include "../Texture/TextureCubeProvider.h"

EnvironmentProvider::~EnvironmentProvider() = default;
EnvironmentProvider::EnvironmentProvider(TextureCubeProvider& cubeProvider, ResourceReleaseBuilder release) noexcept :
    m_cubeProvider{ cubeProvider },
    m_releaseBuilder{ std::move(release) }
{}

std::shared_ptr<IEnvironmentResource> EnvironmentProvider::CreateResource()
{
    return make_shared<EnvironmentResource>();
}

bool EnvironmentProvider::LoadResource(std::shared_ptr<IEnvironmentResource> res, std::shared_ptr<EnvironmentAsset> asset)
{
    if (!asset || !asset->skybox || !asset->reflection)
        return false;

    auto envRes = std::static_pointer_cast<EnvironmentResource>(res);
    if (!envRes)
        return false;

    auto skyboxRes = m_cubeProvider.CreateResource();
    if (!skyboxRes) return false;
    if (!m_cubeProvider.LoadResource(skyboxRes, asset->skybox))
        return false;
    envRes->SetSkybox(skyboxRes);

    auto reflectionRes = m_cubeProvider.CreateResource();
    if (!reflectionRes) return false;
    if (!m_cubeProvider.LoadResource(reflectionRes, asset->reflection))
        return false;
    envRes->SetReflection(reflectionRes);

    envRes->SetIrradianceSH(asset->irradiance->coefficients); //Backend용 Resource로 변환해서 넣어야 하는데 값이 하나 밖에 없어서 일단 직접 데이터만 넣었다.

    m_pendingEnvironments.push_back(envRes);
    return true;
}

void EnvironmentProvider::ReleaseResource(std::shared_ptr<IEnvironmentResource> resource)
{
    if (!resource)
        return;

    m_pendingReleases.emplace_back(std::move(resource));
}

void EnvironmentProvider::Update()
{
    FlushPendingEnvironments();
    FlushPendingRelease();
}

void EnvironmentProvider::FlushPendingEnvironments()
{
    for (auto it = m_pendingEnvironments.begin(); it != m_pendingEnvironments.end();)
    {
        auto& envRes = *it;
        if (!envRes->IsTextureReady())
        {
            ++it;
            continue;
        }
        envRes->MarkReady();
        it = m_pendingEnvironments.erase(it);
    }
}

void EnvironmentProvider::FlushPendingRelease()
{
    if (m_pendingReleases.empty())
        return;

    m_releaseBuilder.ReleaseResources(std::move(m_pendingReleases));
}