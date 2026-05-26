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
        ComPtr<ID3D12Resource> vbRes, ComPtr<ID3D12Resource> ibRes,
        DescriptorAllocation meshTable, UINT vertexCount, UINT indexCount);

private:
    std::unordered_map<uint32_t, std::weak_ptr<IMeshResource>> m_meshes;
};

