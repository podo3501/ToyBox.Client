#include "pch.h"
#include "MeshResource.h"

MeshResource::~MeshResource() = default;
MeshResource::MeshResource() = default;

void MeshResource::SetResource(Resource vb, Resource ib, UINT vertexCount, UINT indexCount) noexcept
{
    m_vb = std::move(vb);
    m_ib = std::move(ib);
    m_vertexCount = vertexCount;
    m_indexCount = indexCount;
}
