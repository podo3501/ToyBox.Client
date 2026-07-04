#pragma once
#include "GameClient/Service/Render/Repository/Mesh/IMeshProvider.h"
#include "MeshGraphBuilder.h"
#include "MeshLoadRequest.h"
#include <queue>

struct MeshLoadRequest;
class DescriptorFactory;
class TaskScheduler;
class ResourceFactory;

class MeshProvider : public IMeshProvider
{
public:
    ~MeshProvider();
    MeshProvider(DescriptorFactory& descFactory, TaskScheduler& taskScheduler, ResourceFactory& resFactory);
    virtual shared_ptr<IMeshResource> CreateResource() override;
    virtual bool LoadResource(std::shared_ptr<IMeshResource> resource, std::shared_ptr<MeshAsset> asset) override;
    virtual void ReleaseResource(std::shared_ptr<IMeshResource> resource) override;

    void Update(size_t uploadBudgetBytes);

private:
    void FlushPendingLoads(size_t uploadBudgetBytes);
    void FlushPendingRelease();

    MeshGraphBuilder m_builder;
    std::queue<MeshLoadRequest> m_pendingLoads;
    std::vector<std::shared_ptr<IMeshResource>> m_pendingReleases;
};
