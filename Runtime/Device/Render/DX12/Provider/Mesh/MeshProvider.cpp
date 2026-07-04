#include "pch.h"
#include "MeshProvider.h"
#include "Resource/Mesh/MeshResource.h"
#include "Factory/DescriptorFactory.h"
#include "Graph/RGTypes.h"
#include "Graph/TaskScheduler.h"
#include "Helpers/CommonHelpers.h"

MeshProvider::~MeshProvider() = default;
MeshProvider::MeshProvider(DescriptorFactory& descFactory, TaskScheduler& taskScheduler, ResourceFactory& resFactory) :
    m_builder{ taskScheduler, resFactory, descFactory }
{}

shared_ptr<IMeshResource> MeshProvider::CreateResource()
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

bool MeshProvider::LoadResource(std::shared_ptr<IMeshResource> resource, std::shared_ptr<MeshAsset> asset)
{
    auto [vbBytes, ibBytes] = EstimateBytes(*asset);

    MeshLoadRequest req;
    req.resource = resource;
    req.asset = asset;
    req.vbBytes = vbBytes;
    req.ibBytes = ibBytes;
    req.estimatedBytes = vbBytes + ibBytes;

    m_pendingLoads.push(req);
    return true;
}

void MeshProvider::ReleaseResource(std::shared_ptr<IMeshResource> resource)
{
    if (!resource)
        return;

    m_pendingReleases.emplace_back(std::move(resource));
}

void MeshProvider::Update(size_t uploadBudgetBytes)
{
    FlushPendingLoads(uploadBudgetBytes);
    FlushPendingRelease();
}

void MeshProvider::FlushPendingLoads(size_t uploadBudgetBytes)
{
    size_t usedBytes = 0;
    std::vector<MeshLoadRequest> batch;
    size_t maxBatch = std::min<size_t>(m_pendingLoads.size(), size_t(64));
    batch.reserve(maxBatch);

    while (!m_pendingLoads.empty())
    {
        MeshLoadRequest req = m_pendingLoads.front();

        if (usedBytes > uploadBudgetBytes && !batch.empty())
            break;

        usedBytes += req.estimatedBytes;
        batch.push_back(std::move(req));
        m_pendingLoads.pop();
    }

    if (batch.empty())
        return;

    m_builder.LoadMeshes(batch);
}

void MeshProvider::FlushPendingRelease()
{
    if (m_pendingReleases.empty())
        return;

    m_builder.ReleaseMeshes(std::move(m_pendingReleases));
}
