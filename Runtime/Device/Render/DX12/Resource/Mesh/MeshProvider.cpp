#include "pch.h"
#include "MeshProvider.h"
#include "MeshResource.h"
#include "MeshGraphBuilder.h"
#include "Factory/DescriptorFactory.h"
#include "Graph/RGTypes.h"
#include "Graph/TaskScheduler.h"
#include "Helpers/CommonHelpers.h"

MeshProvider::~MeshProvider() = default;
MeshProvider::MeshProvider(DescriptorFactory* descFactory, TaskScheduler* taskScheduler, ResourceFactory* resFactory) :
    m_builder{ make_unique<MeshGraphBuilder>(taskScheduler, resFactory, descFactory) }
{}

shared_ptr<IMeshResource> MeshProvider::CreateMeshResource()
{
    return make_shared<MeshResource>();
}

static std::pair<size_t, size_t> EstimateBytes(const MeshAsset& mesh)
{
    size_t vb = mesh.vertices.size();
    size_t ib = mesh.indices.size() * sizeof(uint32_t);

    vb = AlignSize(vb, AlignVertexIndex);
    ib = AlignSize(ib, AlignVertexIndex);

    return { vb, ib };
}

bool MeshProvider::LoadFromAsset(std::shared_ptr<IMeshResource> resource, std::shared_ptr<MeshAsset> asset)
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

void MeshProvider::Update(size_t uploadBudgetBytes)
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
