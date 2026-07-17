#include "pch.h"
#include "TransientMeshProvider.h"
#include "Factory/DescriptorFactory.h"
#include "Resource/Mesh/TransientMeshResource.h"
#include "RenderConstants.h"
#include "Allocator/FrameUploadPools.h"

TransientMeshProvider::TransientMeshProvider(
    FrameUploadPools& frameUploadPools,
    DescriptorFactory& descriptorFactory) noexcept :
    m_frameUploadPools{ frameUploadPools },
    m_descriptorFactory{ descriptorFactory }
{}

std::shared_ptr<TransientMeshResource> TransientMeshProvider::Create(
    std::span<const UIVertex> vertices,
    std::span<const uint32_t> indices)
{
    if (vertices.empty() || indices.empty())
        return nullptr;

    auto resource = std::make_shared<TransientMeshResource>();

    size_t vertexBytes = vertices.size() * sizeof(UIVertex);
    size_t indexBytes = indices.size() * sizeof(uint32_t);

    UploadAllocation vertex = m_frameUploadPools.VertexBuffer().Allocate(
        static_cast<UINT>(vertexBytes), AlignVertexBuffer);
    UploadAllocation index = m_frameUploadPools.IndexBuffer().Allocate(
        static_cast<UINT>(indexBytes), AlignIndexBuffer);

    memcpy(vertex.cpuAddress, vertices.data(), vertexBytes);
    memcpy(index.cpuAddress, indices.data(), indexBytes);

    UINT vertexHeapIndex =
        m_descriptorFactory.CreateBufferSRV(
            DescriptorAllocationType::FrameTransient,
            *vertex.resource,
            static_cast<UINT>(
                vertex.offset / sizeof(UIVertex)),
            static_cast<UINT>(vertices.size()),
            sizeof(UIVertex));
    if (vertexHeapIndex == UINT_MAX)
        return nullptr;

    UINT indexHeapIndex =
        m_descriptorFactory.CreateBufferSRV(
            DescriptorAllocationType::FrameTransient,
            *index.resource,
            static_cast<UINT>(
                index.offset / sizeof(uint32_t)),
            static_cast<UINT>(indices.size()),
            sizeof(uint32_t));
    if (indexHeapIndex == UINT_MAX)
        return nullptr;

    resource->Initialize(
        vertex,
        index,
        static_cast<UINT>(vertices.size()),
        static_cast<UINT>(indices.size()),
        vertexHeapIndex,
        indexHeapIndex);

    return resource;
}