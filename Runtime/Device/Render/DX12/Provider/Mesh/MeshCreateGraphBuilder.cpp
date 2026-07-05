#include "pch.h"
#include "MeshCreateGraphBuilder.h"
#include "Graph/RenderGraph.h"
#include "Graph/TaskScheduler.h"
#include "Factory/DescriptorFactory.h"
#include "Factory/ResourceFactory.h"
#include "MeshLoadRequest.h"
#include "Helpers/CommonHelpers.h"
#include "MeshUtils.h"

struct MeshUploadEntry
{
    RGResourceID resID{ 0 };
    Resource resource{};
    UploadRegion region{};
};

struct MeshFinalizeEntry
{
    std::shared_ptr<MeshAsset> asset{ nullptr };

    RGResourceID meshResID{ 0 };
    RGResourceID vbResID{ 0 };
    RGResourceID ibResID{ 0 };
};

MeshCreateGraphBuilder::~MeshCreateGraphBuilder() = default;
MeshCreateGraphBuilder::MeshCreateGraphBuilder(TaskScheduler& taskScheduler, ResourceFactory& resFactory,
    DescriptorFactory& descFactory) :
    m_taskScheduler{ taskScheduler },
    m_resFactory{ resFactory },
    m_descFactory{ descFactory }
{}

void MeshCreateGraphBuilder::LoadMeshes(
    const std::vector<MeshLoadRequest>& requests)
{
    RenderGraph graph;

    std::vector<MeshUploadEntry> uploads;
    std::vector<MeshFinalizeEntry> finalizes;

    size_t offset = 0;

    for (const auto& req : requests)
    {
        RGResourceID vbResID = RenderGraph::CreateRGResourceID();
        RGResourceID ibResID = RenderGraph::CreateRGResourceID();
        RGResourceID meshResID = RenderGraph::CreateRGResourceID();

        m_registry.Register(meshResID, req.resource);

        auto vbRes = m_resFactory.CreateResource(static_cast<UINT64>(req.vbBytes), ResInitType::Default);
        auto ibRes = m_resFactory.CreateResource(static_cast<UINT64>(req.ibBytes), ResInitType::Default);

        size_t vbOffset = offset;
        size_t ibOffset = offset + req.vbBytes;

        uploads.push_back({
            vbResID, vbRes,
            req.asset->vertices.data(),
            req.asset->vertices.size(),
            static_cast<UINT64>(vbOffset),
            vbRes
            });

        uploads.push_back({
            ibResID, ibRes,
            req.asset->indices.data(),
            req.asset->indices.size() * sizeof(uint32_t),
            static_cast<UINT64>(ibOffset),
            ibRes
            });

        finalizes.push_back({ req.asset, meshResID, vbResID, ibResID });

        offset += req.vbBytes + req.ibBytes;
    }
    RGResourceID uploadResID = RenderGraph::CreateRGResourceID();

    BuildUploadPass(graph, uploads, uploadResID);
    BuildFinalizePass(graph, finalizes);

    auto compiledTasks = graph.Compile();

    size_t totalUploadSize = AlignSize(offset, AlignVertexIndex);
    auto resCtx = std::make_shared<ResourceContext>();
    resCtx->Set(uploadResID, m_resFactory.CreateResource(totalUploadSize, ResInitType::Upload));

    m_taskScheduler.SubmitTask(compiledTasks, resCtx);
}

void MeshCreateGraphBuilder::BuildUploadPass(RenderGraph& graph, std::vector<MeshUploadEntry>& meshUploads, RGResourceID uploadResID)
{
    auto& pass = graph.AddCopyPass("MeshUpload");

    for (auto& mesh : meshUploads)
        pass.Write(mesh.resID, RGAccess::CopyDest);
    
    pass.gpuExecute = 
        [
            this, 
            meshUploads = std::move(meshUploads), 
            uploadResID
        ]
        (CommandList& cmd, TaskContext& ctx) mutable {
        auto& uploadRes = ctx.GetResource(uploadResID);
        for (auto& mesh : meshUploads)
        {
            UploadBufferRegion(cmd, uploadRes, mesh.region);
            ctx.SetResource(mesh.resID, std::move(mesh.resource));
        }
        };
}

void MeshCreateGraphBuilder::BuildFinalizePass(RenderGraph& graph, std::vector<MeshFinalizeEntry>& finalizes)
{
    auto& finalize = graph.AddCpuPass("FinalizeMeshes");

    for (auto& mesh : finalizes)
    {
        finalize.Read(mesh.vbResID, RGAccess::SRV);
        finalize.Read(mesh.ibResID, RGAccess::SRV);
    }
    //?!? 이 finalize.cpuExecute는 m_registry로 옮겨도 되지 않을까? 그래픽 처리라기 보다는 cpu 처리니까. 음.. 생각해보자.
    finalize.cpuExecute = [this, finalizes](TaskContext& ctx) {
        for (auto& mesh : finalizes)
        {
            auto& vb = ctx.GetResource(mesh.vbResID);
            auto& ib = ctx.GetResource(mesh.ibResID);

            auto vertexCount = static_cast<uint32_t>(mesh.asset->vertices.size());
            auto indexCount = static_cast<uint32_t>(mesh.asset->indices.size());

            auto vbHeapIndex = m_descFactory.CreateBufferSRV(vb, mesh.asset->vertexCount, mesh.asset->vertexStride);
            auto ibHeapIndex = m_descFactory.CreateBufferSRV(ib, indexCount, sizeof(uint32_t));

            m_registry.FinalizeMesh(
                mesh.meshResID,
                mesh.asset->format,
                std::move(vb), vbHeapIndex, vertexCount,
                std::move(ib), ibHeapIndex, indexCount);
        }
        };
}