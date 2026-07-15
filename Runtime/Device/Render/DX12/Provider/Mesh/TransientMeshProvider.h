#pragma once
#include "GameClient/Asset/MeshAsset.h"

class TransientMeshResource;
class FrameUploadPools;
class DescriptorFactory;

class TransientMeshProvider
{
public:
    TransientMeshProvider(
        FrameUploadPools& frameUploadPools,
        DescriptorFactory& descriptorFactory) noexcept;

    std::shared_ptr<TransientMeshResource> Create(
        std::span<const UIVertex> vertices,
        std::span<const uint32_t> indices);

private:
    FrameUploadPools& m_frameUploadPools;
    DescriptorFactory& m_descriptorFactory;
};
