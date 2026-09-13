#pragma once
#include "FrameUploadPools.h"
#include "GameClient/Asset/MeshAsset.h"

class Device;
class DescriptorFactory;
class TransientMeshResource;

class TransientMeshProvider
{
public:
    ~TransientMeshProvider();
    explicit TransientMeshProvider(DescriptorFactory& descriptorFactory) noexcept;
    bool Initialize(Device& device);

    std::shared_ptr<TransientMeshResource> Create(
        std::span<const UIVertex> vertices,
        std::span<const uint32_t> indices);

    void ResetFrame(uint32_t slot) noexcept { m_frameUploadPools.Reset(slot); }

private:
    FrameUploadPools m_frameUploadPools;
    DescriptorFactory& m_descriptorFactory;
};
