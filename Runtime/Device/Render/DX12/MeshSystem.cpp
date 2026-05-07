#include "pch.h"
#include "MeshSystem.h"
#include "MeshResource.h"
#include "TaskScheduler.h"
#include "ResourceLoader.h"
#include "MeshGraphBuilder.h"
#include "DescriptorFactory.h"
#include "MeshRegistry.h"
#include "RGTypes.h"

MeshSystem::~MeshSystem() = default;
MeshSystem::MeshSystem(ID3D12Device* device, DescriptorAllocator* srvAllocator, 
    TaskScheduler* taskScheduler, ResourceLoader* loader) :
    m_descriptorFactory{ make_unique<DescriptorFactory>(device, srvAllocator) },
    m_registry{ make_unique<MeshRegistry>() },
    m_builder{ make_unique<MeshGraphBuilder>(taskScheduler, loader, m_descriptorFactory.get(), m_registry.get()) }
{}

shared_ptr<IMeshResource> MeshSystem::CreateMeshResource()
{
    return make_shared<MeshResource>();
}

bool MeshSystem::LoadFromAsset(std::shared_ptr<IMeshResource> resource, std::shared_ptr<MeshAsset> asset)
{
    RGHandle mesh = m_builder->LoadMesh(asset);
    m_registry->Register(mesh.id, resource);

    return true;
}
