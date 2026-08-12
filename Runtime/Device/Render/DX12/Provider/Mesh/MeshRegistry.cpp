#include "pch.h"
#include "MeshRegistry.h"
#include "Resource/Mesh/StaticMeshResource.h"

void MeshRegistry::Register(RGResourceID resID, std::shared_ptr<MeshResource> resource)
{
    m_meshes[resID] = resource;
}

void MeshRegistry::FinalizeMesh(RGResourceID resID, VertexFormat format,
    Resource vRes, UINT vHeapIndex, UINT vCount,
    Resource iRes, UINT iHeapIndex, UINT iCount)
{
    auto it = m_meshes.find(resID);
    if (it == m_meshes.end()) return;

    auto res = it->second.lock();
    if (!res)
        return;

    auto* meshRes = static_cast<StaticMeshResource*>(res.get());
    meshRes->SetVertexFormat(format);
    meshRes->SetResource(std::move(vRes), std::move(iRes), vCount, iCount);
    meshRes->SetVertexHeapIndex(vHeapIndex);
    meshRes->SetIndexHeapIndex(iHeapIndex);
    meshRes->MarkReady();
}