#include "pch.h"
#include "StaticMeshResource.h"

StaticMeshResource::~StaticMeshResource() = default;
StaticMeshResource::StaticMeshResource() = default;

void StaticMeshResource::SetResource(Resource vb, Resource ib, UINT vertexCount, UINT indexCount) noexcept
{
    m_vb = std::move(vb);
    m_ib = std::move(ib);
    m_vertexCount = vertexCount;
    m_indexCount = indexCount;
}