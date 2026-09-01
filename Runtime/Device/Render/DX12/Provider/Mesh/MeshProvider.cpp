#include "pch.h"
#include "MeshProvider.h"
#include "../PendingReleaseQueue.h"
#include "../ProviderBudget.h"
#include "Resource/Mesh/StaticMeshResource.h"
#include "Core/Foundation/Align.h"
#include "Core/Foundation/Cast.hpp"
#include "RenderConstants.h"

MeshProvider::~MeshProvider() = default;
MeshProvider::MeshProvider(
    PendingReleaseQueue& pendingRelease, 
    TaskScheduler& taskScheduler,
    ResourceFactory& resFactory,
    DescriptorFactory& descFactory) noexcept :
    m_pendingRelease{ pendingRelease },
    m_createBuilder{ taskScheduler, resFactory, descFactory }
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

    m_pendingLoads.Push(req);
    return meshRes;
}

void MeshProvider::ReleaseResource(std::shared_ptr<IResource> res)
{
    m_pendingRelease.Add(std::move(res));
}

void MeshProvider::Update(float avgGpuMs)
{
    auto uploadBudgetBytes = ComputeBudget(avgGpuMs, ProviderBudget::Mesh);
    m_pendingLoads.Flush(uploadBudgetBytes, [this](std::vector<MeshLoadRequest>& batch) {
        m_createBuilder.LoadMeshes(batch);
        });
}