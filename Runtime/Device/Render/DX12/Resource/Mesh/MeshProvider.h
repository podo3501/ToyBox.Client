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
    virtual shared_ptr<IMeshResource> CreateMeshResource() override;
    virtual bool LoadFromAsset(std::shared_ptr<IMeshResource> resource, std::shared_ptr<MeshAsset> asset) override;

    void Update(size_t uploadBudgetBytes);

private:
    MeshGraphBuilder m_builder;
    std::queue<MeshLoadRequest> m_pending;
};
