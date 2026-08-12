#pragma once
#include "Resource/Resource.h"
#include "Graph/RGTypes.h"
#include "GameClient/Asset/MeshAsset.h"

class MeshResource;

class MeshRegistry
{
public:
    void Register(RGResourceID resID, std::shared_ptr<MeshResource> resource);
    void FinalizeMesh(RGResourceID resID, VertexFormat format,
        Resource vRes, UINT vHeapIndex, UINT vCount,
        Resource iRes, UINT iHeapIndex, UINT iCount);

private:
    std::unordered_map<RGResourceID, std::weak_ptr<MeshResource>> m_meshes;
};

