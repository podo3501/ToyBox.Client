#pragma once
#include "MeshResource.h"
#include "Resource/Resource.h"
#include "Graph/RGTypes.h"
#include "GameClient/Service/Asset/Assets/MeshAsset.h"

class MeshRegistry
{
public:
    void Register(uint32_t id, std::shared_ptr<IMeshResource> resource);
    void FinalizeMesh(uint32_t id, VertexFormat format,
        Resource vRes, UINT vHeapIndex, UINT vCount,
        Resource iRes, UINT iHeapIndex, UINT iCount);

private:
    std::unordered_map<uint32_t, std::weak_ptr<IMeshResource>> m_meshes;
};

