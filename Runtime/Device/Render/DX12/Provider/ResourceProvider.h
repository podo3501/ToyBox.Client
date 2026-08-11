#pragma once
#include "GameClient/Service/Render/Repository/IResourceProvider.h"
#include "ResourceReleaseBuilder.h"
#include "Texture/TextureProvider.h"

class TaskScheduler;
class IPendingResource;

class ResourceProvider : public IResourceProvider
{
public:
    virtual ~ResourceProvider();
    ResourceProvider() = delete;
    explicit ResourceProvider(TaskScheduler& taskScheduler) noexcept;
    virtual std::shared_ptr<IResource> CreateResource(std::shared_ptr<AssetData> asset) = 0;
    virtual void ReleaseResource(std::shared_ptr<IResource> res) override;

protected:
    void FlushPendingLoad();
    void FlushPendingRelease();

    ResourceReleaseBuilder m_releaseBuilder;
    
    std::vector<std::shared_ptr<IPendingResource>> m_pendingLoads;
    std::vector<std::shared_ptr<IResource>> m_pendingReleases;
};