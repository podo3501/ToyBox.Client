#pragma once
#include "GameClient/Service/Render/Repository/IMeshSystem.h"

struct ID3D12Device;
class DescriptorAllocator;
class DescriptorFactory;
class MeshGraphBuilder;
class MeshRegistry;
class TaskScheduler;
class ResourceLoader;

class MeshSystem : public IMeshSystem
{
public:
    ~MeshSystem();
    MeshSystem(ID3D12Device* device, DescriptorAllocator* srvAllocator, TaskScheduler* taskScheduler, ResourceLoader* loader);
    virtual shared_ptr<IMeshResource> CreateMeshResource() override;
    virtual bool LoadFromAsset(std::shared_ptr<IMeshResource> resource, std::shared_ptr<MeshAsset> asset) override;

private:
    unique_ptr<DescriptorFactory> m_descriptorFactory;
    unique_ptr<MeshRegistry> m_registry;
    unique_ptr<MeshGraphBuilder> m_builder;
};
