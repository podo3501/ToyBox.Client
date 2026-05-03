#include "pch.h"
#include "MeshRegistry.h"

void MeshRegistry::Register(uint32_t id, std::shared_ptr<IMeshResource> resource)
{
    m_meshes[id] = resource;
}

void MeshRegistry::FinalizeMesh(uint32_t id, 
    ComPtr<ID3D12Resource> vbRes, DescriptorAllocation vbAlloc,
    ComPtr<ID3D12Resource> ibRes, DescriptorAllocation ibAlloc, UINT indexCount)
{
    auto it = m_meshes.find(id);
    if (it == m_meshes.end()) return;

    auto res = it->second.lock();
    if (!res)
        return;

    auto* meshRes = static_cast<MeshResource*>(res.get());
    meshRes->SetResource(std::move(vbRes), std::move(ibRes), indexCount);
    meshRes->SetSRV(std::move(vbAlloc), std::move(ibAlloc));
    meshRes->MarkReady();
}