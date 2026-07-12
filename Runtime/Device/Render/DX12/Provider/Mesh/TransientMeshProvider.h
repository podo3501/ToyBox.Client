#pragma once
#include "GameClient/Asset/MeshAsset.h"

class TransientMeshResource;
class FrameUploadAllocator;
class DescriptorFactory;

class TransientMeshProvider
{
public:
    TransientMeshProvider(
        FrameUploadAllocator& allocator,
        DescriptorFactory& descriptorFactory) noexcept;

    std::shared_ptr<TransientMeshResource> Create(
        std::span<const UIVertex> vertices,
        std::span<const uint32_t> indices);

private:
    FrameUploadAllocator& m_allocator;
    DescriptorFactory& m_descriptorFactory;
};
