#pragma once
#include "MeshResource.h"
#include "Allocator/UploadAllocation.h"

class TransientMeshResource : public MeshResource
{
public:
    TransientMeshResource() = default;
    ~TransientMeshResource() override = default;
  
    virtual UINT GetVertexHeapIndex() const noexcept override { return m_vertexHeapIndex; }
    virtual UINT GetIndexHeapIndex() const noexcept override { return m_indexHeapIndex; }
    virtual UINT GetVertexCount() const noexcept override { return m_vertexCount; }
    virtual UINT GetIndexCount() const noexcept override { return m_indexCount; }

    void Initialize(
        const UploadAllocation& vertex,
        const UploadAllocation& index,
        UINT vertexCount,
        UINT indexCount,
        UINT vertexHeapIndex,
        UINT indexHeapIndex) noexcept;

private:
    UploadAllocation m_vertexAllocation{};
    UploadAllocation m_indexAllocation{};

    UINT m_vertexHeapIndex{};
    UINT m_indexHeapIndex{};

    UINT m_vertexCount{};
    UINT m_indexCount{};
};