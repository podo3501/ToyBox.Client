#include "pch.h"
#include "MeshResource.h"

MeshResource::~MeshResource()
{
    int a = 1;
}

MeshResource::MeshResource() = default;

void MeshResource::SetResource(ComPtr<ID3D12Resource> vb, ComPtr<ID3D12Resource> ib, UINT indexCount)
{
    m_vb = std::move(vb);
    m_ib = std::move(ib);
    m_indexCount = indexCount;
}

void MeshResource::SetSRV(DescriptorAllocation vbAlloc, DescriptorAllocation ibAlloc)
{
    m_vbSrv = std::move(vbAlloc);
    m_ibSrv = std::move(ibAlloc);
}
   
