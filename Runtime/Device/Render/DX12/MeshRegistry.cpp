#include "pch.h"
#include "MeshRegistry.h"

void MeshRegistry::Register(uint32_t id, std::shared_ptr<IMeshResource> resource)
{
    m_meshes[id] = resource;
}

void MeshRegistry::FinalizeMesh(uint32_t id, VertexFormat format,
    ComPtr<ID3D12Resource> vbRes, ComPtr<ID3D12Resource> ibRes, 
    DescriptorAllocation meshTable, UINT vertexCount, UINT indexCount)
{
    auto it = m_meshes.find(id);
    if (it == m_meshes.end()) return;

    auto res = it->second.lock();
    if (!res)
        return;

    auto* meshRes = static_cast<MeshResource*>(res.get());
    meshRes->SetVertexFormat(format);
    meshRes->SetResource(std::move(vbRes), std::move(ibRes), vertexCount, indexCount);
    meshRes->SetMeshTable(std::move(meshTable));
    meshRes->MarkReady();
}