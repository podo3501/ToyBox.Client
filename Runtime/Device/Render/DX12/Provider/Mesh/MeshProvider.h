#pragma once
#include "GameClient/Service/Render/Repository/IResourceProvider.h"
#include "../IUpdatableProvider.h"
#include "MeshCreateGraphBuilder.h"
#include "../PendingUploadQueue.h"
#include "MeshLoadRequest.h"
#include <queue>

struct MeshLoadRequest;
class PendingReleaseQueue;

class MeshProvider : public IResourceProvider, public IUpdatableProvider
{
public:
    ~MeshProvider();
    MeshProvider() = delete;
    MeshProvider(
        PendingReleaseQueue& pendingRelease, 
        TaskScheduler& taskScheduler,
        ResourceFactory& resFactory,
        DescriptorFactory& descFactory) noexcept;
    virtual std::shared_ptr<IResource> CreateResource(std::shared_ptr<AssetData> asset) override;
    virtual void ReleaseResource(std::shared_ptr<IResource> res) override;
    virtual void Update(float avgGpuMs) override;

private:
    void FlushPendingRelease();

    PendingReleaseQueue& m_pendingRelease;
    MeshCreateGraphBuilder m_createBuilder;
    PendingUploadQueue<MeshLoadRequest> m_pendingLoads; //그래프 콜백에서 직접 MarkReady
};
