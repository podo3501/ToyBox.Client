#pragma once
#include "GameClient/Service/Render/Repository/IResourceProvider.h"
#include "../PendingLoadQueue.h"
#include "../PendingReleaseQueue.h"

struct TextureCubeAsset;
class TextureCubeProvider;
class TextureCubeResource;

class EnvironmentProvider : public IResourceProvider
{
public:
    ~EnvironmentProvider();
    EnvironmentProvider(TaskScheduler& taskScheduler, TextureCubeProvider& cubeProvider) noexcept;
    virtual std::shared_ptr<IResource> CreateResource(std::shared_ptr<AssetData> asset) override;
    virtual void ReleaseResource(std::shared_ptr<IResource> res) override;
    void Update();

private:
    std::shared_ptr<TextureCubeResource> CreateCubeResource(std::shared_ptr<TextureCubeAsset> cubeAsset);

    PendingLoadQueue m_pendingLoad;
    PendingReleaseQueue m_pendingRelease;
    TextureCubeProvider& m_cubeProvider;
};