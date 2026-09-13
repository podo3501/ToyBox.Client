#include "pch.h"
#include "TransientMeshProvider.h"
#include "Factory/DescriptorFactory.h"
#include "Resource/Mesh/TransientMeshResource.h"
#include "RenderConstants.h"

TransientMeshProvider::~TransientMeshProvider() = default;
TransientMeshProvider::TransientMeshProvider(
    DescriptorFactory& descriptorFactory) noexcept :
    m_descriptorFactory{ descriptorFactory }
{}

bool TransientMeshProvider::Initialize(Device& device)
{
    FrameUploadPoolDesc poolDescs[] =
    {
        { FrameUploadPoolType::UIVertex, 16 * 1024 * 1024, sizeof(UIVertex) },
        { FrameUploadPoolType::UIIndex,  16 * 1024 * 1024, sizeof(uint32_t) },
    };

    return m_frameUploadPools.Initialize(device, poolDescs);
}

std::shared_ptr<TransientMeshResource> TransientMeshProvider::Create(
    std::span<const UIVertex> vertices,
    std::span<const uint32_t> indices)
{
    if (vertices.empty() || indices.empty())
        return nullptr;

    auto resource = std::make_shared<TransientMeshResource>();

    UploadAllocation vertex = m_frameUploadPools.Allocate(
        FrameUploadPoolType::UIVertex, 
        static_cast<UINT>(vertices.size()));

    UploadAllocation index = m_frameUploadPools.Allocate(
        FrameUploadPoolType::UIIndex, 
        static_cast<UINT>(indices.size()));

    // 검증: offset이 각 원소 크기의 배수가 아니면 FirstElement 계산이 틀어짐
    Assert(vertex.offset % sizeof(UIVertex) == 0);
    Assert(index.offset % sizeof(uint32_t) == 0);

    memcpy(vertex.cpuAddress, vertices.data(), vertex.sizeInBytes);
    memcpy(index.cpuAddress, indices.data(), index.sizeInBytes);

    UINT vertexHeapIndex =
        m_descriptorFactory.CreateBufferSRV(
            DescriptorAllocationType::Transient,
            *vertex.resource,
            static_cast<UINT>(
                vertex.offset / sizeof(UIVertex)),
            static_cast<UINT>(vertices.size()),
            sizeof(UIVertex));
    if (vertexHeapIndex == UINT_MAX)
        return nullptr;

    UINT indexHeapIndex =
        m_descriptorFactory.CreateBufferSRV(
            DescriptorAllocationType::Transient,
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