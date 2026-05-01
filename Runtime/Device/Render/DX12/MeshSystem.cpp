#include "pch.h"
#include "MeshSystem.h"
#include "MeshResource.h"
#include "MeshGraphBuilder.h"
#include "RGTypes.h"

MeshSystem::~MeshSystem() = default;
MeshSystem::MeshSystem(MeshGraphBuilder* builder, MeshRegistry* registry) :
    m_builder{ make_unique<MeshGraphBuilder>() }
    //m_registry{ registry }
{}

shared_ptr<IMeshResource> MeshSystem::CreateMeshResource()
{
    return make_shared<MeshResource>();
}

bool MeshSystem::LoadFromAsset(std::shared_ptr<IMeshResource> resource, std::shared_ptr<MeshAsset> asset)
{
    RGResource mesh = m_builder->LoadMesh(asset);
    //m_registry->Register(tex.id, resource);

    return true;
}
