#pragma once
#include "Resource/Resource.h"
#include "Graph/RGTypes.h"
#include "GameClient/Asset/MeshAsset.h"

struct IMeshResource;

class MeshRegistry
{
public:
    void Register(RGResourceID resID, std::shared_ptr<IMeshResource> resource);
    void FinalizeMesh(RGResourceID resID, VertexFormat format,
        Resource vRes, UINT vHeapIndex, UINT vCount,
        Resource iRes, UINT iHeapIndex, UINT iCount);

private:
    std::unordered_map<RGResourceID, std::weak_ptr<IMeshResource>> m_meshes;
};

