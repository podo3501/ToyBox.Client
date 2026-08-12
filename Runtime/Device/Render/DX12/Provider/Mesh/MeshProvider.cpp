#include "pch.h"
#include "MeshProvider.h"
#include "Resource/Mesh/StaticMeshResource.h"
#include "Core/Foundation/Align.h"
#include "Core/Foundation/Cast.hpp"
#include "RenderConstants.h"

MeshProvider::~MeshProvider() = default;
MeshProvider::MeshProvider(TaskScheduler& taskScheduler, MeshCreateGraphBuilder create) noexcept :
    m_pendingRelease{ taskScheduler },
    m_createBuilder{ std::move(create) }
{}

static std::pair<size_t, size_t> EstimateBytes(MeshAsset* mesh)
{
    size_t vb = mesh->vertices.size();
    size_t ib = mesh->indices.size() * sizeof(uint32_t);

    vb = Core::AlignUp(vb, AlignVertexBuffer);
    ib = Core::AlignUp(ib, AlignIndexBuffer);

    return { vb, ib };
}

std::shared_ptr<IResource> MeshProvider::CreateResource(std::shared_ptr<AssetData> asset)
{
    if (!asset) return nullptr;

    auto meshAsset = Core::Cast<MeshAsset>(asset);
    if (!meshAsset) return nullptr;

    auto [vbBytes, ibBytes] = EstimateBytes(meshAsset.get());
    auto meshRes = make_shared<StaticMeshResource>();

    MeshLoadRequest req;
    req.resource = meshRes;
    req.asset = meshAsset;
    req.vbBytes = vbBytes;
    req.ibBytes = ibBytes;
    req.estimatedBytes = vbBytes + ibBytes;

    m_pendingLoads.push(req);
    return meshRes;
}

void MeshProvider::ReleaseResource(std::shared_ptr<IResource> res)
{
    if (!res)
        return;

    m_pendingRelease.Add(std::move(res));
}

void MeshProvider::Update(size_t uploadBudgetBytes)
{
    FlushPendingLoads(uploadBudgetBytes);
    m_pendingRelease.Flush();
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

    m_createBuilder.LoadMeshes(batch);
}