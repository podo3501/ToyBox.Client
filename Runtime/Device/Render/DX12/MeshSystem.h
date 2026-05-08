#pragma once
#include "GameClient/Service/Render/Repository/IMeshSystem.h"
#include "MeshLoadRequest.h"
#include <queue>

struct ID3D12Device;
struct MeshLoadRequest;
class DescriptorAllocator;
class DescriptorFactory;
class MeshGraphBuilder;
class TaskScheduler;
class ResourceLoader;

class MeshSystem : public IMeshSystem
{
public:
    ~MeshSystem();
    MeshSystem(ID3D12Device* device, DescriptorAllocator* srvAllocator, TaskScheduler* taskScheduler, ResourceLoader* loader);
    virtual shared_ptr<IMeshResource> CreateMeshResource() override;
    virtual bool LoadFromAsset(std::shared_ptr<IMeshResource> resource, std::shared_ptr<MeshAsset> asset) override;

    void Update(size_t uploadBudgetBytes);

private:
    unique_ptr<DescriptorFactory> m_descriptorFactory;
    unique_ptr<MeshGraphBuilder> m_builder;

    std::queue<MeshLoadRequest> m_pending;
};
