#pragma once
#include "MeshResource.h"
#include "Descriptor/DescriptorAllocation.h"
#include "RGTypes.h"
#include "GameClient/Service/Asset/Assets/MeshAsset.h"

class MeshRegistry
{
public:
    void Register(uint32_t id, std::shared_ptr<IMeshResource> resource);
    void FinalizeMesh(uint32_t id, VertexFormat format,
        ComPtr<ID3D12Resource> vRes, UINT vHeapIndex, UINT vCount,
        ComPtr<ID3D12Resource> iRes, UINT iHeapIndex, UINT iCount);

private:
    std::unordered_map<uint32_t, std::weak_ptr<IMeshResource>> m_meshes;
};

