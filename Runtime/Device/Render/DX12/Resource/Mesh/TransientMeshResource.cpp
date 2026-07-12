#include "pch.h"
#include "TransientMeshResource.h"

void TransientMeshResource::Initialize(
    const UploadAllocation& vertex,
    const UploadAllocation& index,
    UINT vertexCount,
    UINT indexCount,
    UINT vertexHeapIndex,
    UINT indexHeapIndex) noexcept
{
    m_vertexAllocation = vertex;
    m_indexAllocation = index;

    m_vertexCount = vertexCount;
    m_indexCount = indexCount;

    m_vertexHeapIndex = vertexHeapIndex;
    m_indexHeapIndex = indexHeapIndex;

    MarkReady();
}