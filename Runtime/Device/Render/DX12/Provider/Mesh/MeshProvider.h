#pragma once
#include "GameClient/Service/Render/Repository/IResourceProvider.h"
#include "MeshCreateGraphBuilder.h"
#include "../PendingReleaseQueue.h"
#include "MeshLoadRequest.h"
#include <queue>

struct MeshLoadRequest;

class MeshProvider : public IResourceProvider
{
public:
    ~MeshProvider();
    MeshProvider() = delete;
    MeshProvider(TaskScheduler& taskScheduler, MeshCreateGraphBuilder create) noexcept;
    virtual std::shared_ptr<IResource> CreateResource(std::shared_ptr<AssetData> asset) override;
    virtual void ReleaseResource(std::shared_ptr<IResource> res) override;

    void Update(size_t uploadBudgetBytes);

private:
    void FlushPendingLoads(size_t uploadBudgetBytes);
    void FlushPendingRelease();

    MeshCreateGraphBuilder m_createBuilder;
    PendingReleaseQueue m_pendingRelease;
    std::queue<MeshLoadRequest> m_pendingLoads; // PendingLoadQueue는 안가짐. 그래프 콜백에서 직접 MarkReady
};
