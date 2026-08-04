#pragma once
#include "GameClient/Service/Render/Repository/Environment/IEnvironmentProvider.h"
#include "../ResourceReleaseBuilder.h"

class EnvironmentResource;
class TextureCubeProvider;

class EnvironmentProvider : public IEnvironmentProvider
{
public:
    ~EnvironmentProvider();
    EnvironmentProvider(TextureCubeProvider& cubeProvider, ResourceReleaseBuilder release) noexcept;

    std::shared_ptr<IEnvironmentResource> CreateResource() override;
    virtual bool LoadResource(std::shared_ptr<IEnvironmentResource> res, std::shared_ptr<EnvironmentAsset> asset) override;
    virtual void ReleaseResource(std::shared_ptr<IEnvironmentResource> res) override;
    void Update();

private:
    void FlushPendingEnvironments();
    void FlushPendingRelease();

    TextureCubeProvider& m_cubeProvider;
    ResourceReleaseBuilder m_releaseBuilder;

    std::vector<std::shared_ptr<EnvironmentResource>> m_pendingEnvironments;
    std::vector<std::shared_ptr<IEnvironmentResource>> m_pendingReleases;
};