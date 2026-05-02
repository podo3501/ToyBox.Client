#pragma once
#include "MeshResource.h"
#include "DescriptorAllocation.h"
#include "RGTypes.h"

class MeshRegistry
{
public:
    void Register(uint32_t id, std::shared_ptr<IMeshResource> resource);
    void FinalizeMesh(uint32_t id, ComPtr<ID3D12Resource> resource, DescriptorAllocation alloc);

private:
    std::unordered_map<uint32_t, std::weak_ptr<IMeshResource>> m_meshes;
};

