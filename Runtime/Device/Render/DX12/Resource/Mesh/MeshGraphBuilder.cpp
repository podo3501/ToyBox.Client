#include "pch.h"
#include "MeshGraphBuilder.h"
#include "Graph/RenderGraph.h"
#include "Graph/RenderPass.h"
#include "Graph/TaskScheduler.h"
#include "Resource/Resource.h"
#include "Factory/DescriptorFactory.h"
#include "Factory/ResourceFactory.h"
#include "MeshLoadRequest.h"
#include "Helpers/CommonHelpers.h"
#include "GameClient/Service/Asset/Assets/MeshAsset.h"
#include "MeshUtils.h"

struct MeshUploadEntry
{
    RGHandle handle;
    Resource resource;
    UploadRegion region;
};

struct MeshFinalizeEntry
{
    std::shared_ptr<MeshAsset> asset;

    RGHandle hMesh;
    RGHandle hVb;
    RGHandle hIb;
};

MeshGraphBuilder::~MeshGraphBuilder() = default;
MeshGraphBuilder::MeshGraphBuilder(TaskScheduler& taskScheduler, ResourceFactory& resFactory,
    DescriptorFactory& descFactory) :
    m_taskScheduler{ taskScheduler },
    m_resFactory{ resFactory },
    m_descFactory{ descFactory }
{}

void MeshGraphBuilder::LoadMeshes(
    const std::vector<MeshLoadRequest>& requests)
{
    RenderGraph graph;

    std::vector<MeshUploadEntry> uploads;
    std::vector<MeshFinalizeEntry> finalizes;

    size_t offset = 0;

    for (const auto& req : requests)
    {
        RGHandle hVb = CreateRGHandle();
        RGHandle hIb = CreateRGHandle();
        RGHandle hMesh = CreateRGHandle();

        m_registry.Register(hMesh.id, req.resource);

        auto vbRes = m_resFactory.CreateBufferResource(static_cast<UINT64>(req.vbBytes));
        auto ibRes = m_resFactory.CreateBufferResource(static_cast<UINT64>(req.ibBytes));

        size_t vbOffset = offset;
        size_t ibOffset = offset + req.vbBytes;

        uploads.push_back({
            hVb, vbRes,
            req.asset->vertices.data(),
            req.asset->vertices.size(),
            static_cast<UINT64>(vbOffset),
            vbRes
            });

        uploads.push_back({
            hIb, ibRes,
            req.asset->indices.data(),
            req.asset->indices.size() * sizeof(uint32_t),
            static_cast<UINT64>(ibOffset),
            ibRes
            });

        finalizes.push_back({ req.asset, hMesh, hVb, hIb });

        offset += req.vbBytes + req.ibBytes;
    }
    RGHandle hUploadRes = CreateRGHandle();

    BuildUploadPass(graph, uploads, hUploadRes);
    BuildFinalizePass(graph, finalizes);

    auto compiledTasks = graph.Compile();

    size_t totalUploadSize = AlignSize(offset, AlignVertexIndex);
    auto resCtx = std::make_shared<ResourceContext>();
    resCtx->Set(hUploadRes, m_resFactory.CreateUploadResource(totalUploadSize));

    m_taskScheduler.Submit(compiledTasks, resCtx);
}

void MeshGraphBuilder::BuildUploadPass(RenderGraph& graph, std::vector<MeshUploadEntry>& meshUploads, RGHandle hUploadRes)
{
    auto& pass = graph.AddPass("MeshUpload", CommandType::Copy);

    for (auto& mesh : meshUploads)
        pass.writes.push_back({ mesh.handle, RGAccess::CopyDest });
    
    pass.gpuExecute = [this, meshUploads, hUploadRes](CommandList& cmd, TaskContext& ctx) mutable {
        auto& uploadRes = ctx.GetResource(hUploadRes);
        for (auto& mesh : meshUploads)
        {
            UploadBufferRegion(cmd, uploadRes, mesh.region);
            ctx.SetResource(mesh.handle, std::move(mesh.resource));
        }
        };
}

void MeshGraphBuilder::BuildFinalizePass(RenderGraph& graph, std::vector<MeshFinalizeEntry>& finalizes)
{
    auto& finalize = graph.AddPass("FinalizeMeshes", CommandType::None);

    for (auto& mesh : finalizes)
    {
        finalize.reads.push_back({ mesh.hVb, RGAccess::CopyDest });
        finalize.reads.push_back({ mesh.hIb, RGAccess::CopyDest });
        finalize.writes.push_back({ mesh.hVb, RGAccess::SRV });
        finalize.writes.push_back({ mesh.hIb, RGAccess::SRV });
    }

    finalize.cpuExecute = [this, finalizes](TaskContext& ctx) {
        for (auto& mesh : finalizes)
        {
            auto& vb = ctx.GetResource(mesh.hVb);
            auto& ib = ctx.GetResource(mesh.hIb);

            auto vertexCount = static_cast<uint32_t>(mesh.asset->vertices.size());
            auto indexCount = static_cast<uint32_t>(mesh.asset->indices.size());

            auto vbHeapIndex = m_descFactory.CreateBufferSRV(vb, mesh.asset->vertexCount, mesh.asset->vertexStride);
            auto ibHeapIndex = m_descFactory.CreateBufferSRV(ib, indexCount, sizeof(uint32_t));

            m_registry.FinalizeMesh(
                mesh.hMesh.id,
                mesh.asset->format,
                std::move(vb), vbHeapIndex, vertexCount,
                std::move(ib), ibHeapIndex, indexCount);
        }
        };
}

RGHandle MeshGraphBuilder::CreateRGHandle()
{
    RGHandle handle{ m_nextId++ };
    return handle;
}