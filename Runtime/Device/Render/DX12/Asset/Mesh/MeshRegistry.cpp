#include "pch.h"
#include "MeshRegistry.h"

void MeshRegistry::Register(uint32_t id, std::shared_ptr<IMeshResource> resource)
{
    m_meshes[id] = resource;
}

void MeshRegistry::FinalizeMesh(uint32_t id, VertexFormat format,
    Resource vRes, UINT vHeapIndex, UINT vCount,
    Resource iRes, UINT iHeapIndex, UINT iCount)
{
    auto it = m_meshes.find(id);
    if (it == m_meshes.end()) return;

    auto res = it->second.lock();
    if (!res)
        return;

    auto* meshRes = static_cast<MeshResource*>(res.get());
    meshRes->SetVertexFormat(format);
    meshRes->SetResource(std::move(vRes), std::move(iRes), vCount, iCount);
    meshRes->SetVertexHeapIndex(vHeapIndex);
    meshRes->SetIndexHeapIndex(iHeapIndex);
    meshRes->MarkReady();
}