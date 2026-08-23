#pragma once
#include "GameClient/Service/Render/Repository/IResourceProvider.h"
#include "../IUpdatableProvider.h"

struct TextureCubeAsset;
class PendingLoadQueue;
class PendingReleaseQueue;
class TextureCubeProvider;
class TextureCubeResource;

class EnvironmentProvider : public IResourceProvider
{
public:
    ~EnvironmentProvider();
    EnvironmentProvider(
        PendingLoadQueue& pendingLoad,
        PendingReleaseQueue& pendingRelease, 
        TextureCubeProvider& cubeProvider) noexcept;
    virtual std::shared_ptr<IResource> CreateResource(std::shared_ptr<AssetData> asset) override;
    virtual void ReleaseResource(std::shared_ptr<IResource> res) override;

private:
    std::shared_ptr<TextureCubeResource> CreateCubeResource(std::shared_ptr<TextureCubeAsset> cubeAsset);

    PendingLoadQueue& m_pendingLoad;
    PendingReleaseQueue& m_pendingRelease;
    TextureCubeProvider& m_cubeProvider;
};