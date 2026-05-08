#include "pch.h"
#include "MeshSystem.h"
#include "MeshResource.h"
#include "TaskScheduler.h"
#include "ResourceLoader.h"
#include "MeshGraphBuilder.h"
#include "DescriptorFactory.h"
#include "RGTypes.h"
#include "DX12Utils.h"

MeshSystem::~MeshSystem() = default;
MeshSystem::MeshSystem(ID3D12Device* device, DescriptorAllocator* srvAllocator, 
    TaskScheduler* taskScheduler, ResourceLoader* loader) :
    m_descriptorFactory{ make_unique<DescriptorFactory>(device, srvAllocator) },
    m_builder{ make_unique<MeshGraphBuilder>(taskScheduler, loader, m_descriptorFactory.get()) }
{}

shared_ptr<IMeshResource> MeshSystem::CreateMeshResource()
{
    return make_shared<MeshResource>();
}

static std::pair<size_t, size_t> EstimateBytes(const MeshAsset& mesh)
{
    size_t vb = mesh.vertices.size() * sizeof(Vertex);
    size_t ib = mesh.indices.size() * sizeof(uint32_t);

    vb = AlignSize(vb, AlignVertexIndex);
    ib = AlignSize(ib, AlignVertexIndex);

    return { vb, ib };
}

bool MeshSystem::LoadFromAsset(std::shared_ptr<IMeshResource> resource, std::shared_ptr<MeshAsset> asset)
{
    auto [vbBytes, ibBytes] = EstimateBytes(*asset);

    MeshLoadRequest req;
    req.resource = resource;
    req.asset = asset;
    req.vbBytes = vbBytes;
    req.ibBytes = ibBytes;
    req.estimatedBytes = vbBytes + ibBytes;

    m_pending.push(req);
    return true;
}

void MeshSystem::Update(size_t uploadBudgetBytes)
{
    size_t usedBytes = 0;
    std::vector<MeshLoadRequest> batch;
    batch.reserve(32);

    while (!m_pending.empty())
    {
        auto& req = m_pending.front();

        if (usedBytes > uploadBudgetBytes && !batch.empty())
            break;

        usedBytes += req.estimatedBytes;
        batch.push_back(req);
        m_pending.pop();
    }

    if (batch.empty())
        return;

    m_builder->LoadMeshes(batch);
}
