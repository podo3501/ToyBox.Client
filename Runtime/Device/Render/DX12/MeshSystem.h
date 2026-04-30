#pragma once
#include "GameClient/Service/Render/Repository/IMeshSystem.h"

class MeshGraphBuilder;
class MeshRegistry;

class MeshSystem : public IMeshSystem
{
public:
    ~MeshSystem();
    MeshSystem(MeshGraphBuilder* builder, MeshRegistry* registry);
    virtual shared_ptr<IMeshResource> CreateMeshResource() override;
    virtual bool LoadFromAsset(std::shared_ptr<IMeshResource> resource, std::shared_ptr<MeshAsset> asset) override;

private:
    //TextureGraphBuilder* m_builder{ nullptr };
    //TextureRegistry* m_registry{ nullptr };
};
