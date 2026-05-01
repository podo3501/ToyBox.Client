#include "pch.h"
#include "MeshResource.h"

MeshResource::~MeshResource() = default;
MeshResource::MeshResource() = default;
void MeshResource::SetResource(
    ComPtr<ID3D12Resource> vb, UINT vbSize, UINT stride,
    ComPtr<ID3D12Resource> ib, UINT indexCount, DXGI_FORMAT indexFormat)
{
    m_vb = std::move(vb);
    m_ib = std::move(ib);
    m_indexCount = indexCount;

    m_vbView.BufferLocation = m_vb->GetGPUVirtualAddress();
    m_vbView.SizeInBytes = vbSize;
    m_vbView.StrideInBytes = stride;

    m_ibView.BufferLocation = m_ib->GetGPUVirtualAddress();
    m_ibView.SizeInBytes = indexCount *
        (indexFormat == DXGI_FORMAT_R16_UINT ? 2 : 4);
    m_ibView.Format = indexFormat;
}