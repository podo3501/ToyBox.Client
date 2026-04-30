#include "pch.h"
#include "MeshSystem.h"
#include "MeshResource.h"
//#include "TextureGraphBuilder.h"

MeshSystem::~MeshSystem() = default;
MeshSystem::MeshSystem(MeshGraphBuilder* builder, MeshRegistry* registry) {}
    //m_builder{ builder },
    //m_registry{ registry }
//{}

shared_ptr<IMeshResource> MeshSystem::CreateMeshResource()
{
    return make_shared<MeshResource>();
}

bool MeshSystem::LoadFromAsset(std::shared_ptr<IMeshResource> resource, std::shared_ptr<MeshAsset> asset)
{
    //RGTexture tex = m_builder->LoadTexture(asset, desc);
    //m_registry->Register(tex.id, resource);

    return true;
}
