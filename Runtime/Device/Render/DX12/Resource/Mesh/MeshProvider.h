#pragma once
#include "GameClient/Service/Render/Repository/Mesh/IMeshProvider.h"
#include "MeshLoadRequest.h"
#include <queue>

struct ID3D12Device;
struct MeshLoadRequest;
class DescriptorAllocator;
class DescriptorFactory;
class MeshGraphBuilder;
class TaskScheduler;
class ResourceLoader;

class MeshProvider : public IMeshProvider
{
public:
    ~MeshProvider();
    MeshProvider(ID3D12Device* device, DescriptorFactory* descFactory, TaskScheduler* taskScheduler, ResourceLoader* loader);
    virtual shared_ptr<IMeshResource> CreateMeshResource() override;
    virtual bool LoadFromAsset(std::shared_ptr<IMeshResource> resource, std::shared_ptr<MeshAsset> asset) override;

    void Update(size_t uploadBudgetBytes);

private:
    unique_ptr<MeshGraphBuilder> m_builder;
    std::queue<MeshLoadRequest> m_pending;
};
