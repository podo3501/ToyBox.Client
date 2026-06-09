#include "pch.h"
#include "MeshGraphBuilder.h"
#include "RenderGraph.h"
#include "RenderPass.h"
#include "TaskScheduler.h"
#include "ResourceLoader.h"
#include "Descriptor/DescriptorFactory.h"
#include "MeshRegistry.h"
#include "MeshLoadRequest.h"
#include "Helpers/CommonHelpers.h"
#include "GameClient/Service/Asset/Assets/MeshAsset.h"

struct MeshUploadEntry
{
    RGHandle handle;
    ComPtr<ID3D12Resource> resource;
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
MeshGraphBuilder::MeshGraphBuilder(TaskScheduler* taskScheduler, ResourceLoader* loader,
    DescriptorFactory* descFactory) :
    m_taskScheduler{ taskScheduler },
    m_loader{ loader },
    m_descFactory{ descFactory },
    m_registry{ make_unique<MeshRegistry>() }
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

        m_registry->Register(hMesh.id, req.resource);

        auto vbRes = m_loader->CreateBufferResource(static_cast<UINT64>(req.vbBytes));
        auto ibRes = m_loader->CreateBufferResource(static_cast<UINT64>(req.ibBytes));

        size_t vbOffset = offset;
        size_t ibOffset = offset + req.vbBytes;

        uploads.push_back({
            hVb, vbRes,
            req.asset->vertices.data(),
            req.asset->vertices.size(),
            static_cast<UINT64>(vbOffset),
            vbRes.Get()
            });

        uploads.push_back({
            hIb, ibRes,
            req.asset->indices.data(),
            req.asset->indices.size() * sizeof(uint32_t),
            static_cast<UINT64>(ibOffset),
            ibRes.Get()
            });

        finalizes.push_back({ req.asset, hMesh, hVb, hIb });

        offset += req.vbBytes + req.ibBytes;
    }

    BuildUploadPass(graph, uploads);
    BuildFinalizePass(graph, finalizes);

    auto compiledTasks = graph.Compile();
    size_t totalUploadSize = AlignSize(offset, AlignVertexIndex);

    auto uploadCtx = std::make_shared<UploadContext>();
    uploadCtx->resource = m_loader->CreateUploadResource(totalUploadSize);

    m_taskScheduler->Submit(compiledTasks, std::make_shared<ResourceContext>(), uploadCtx);
}

void MeshGraphBuilder::BuildUploadPass(RenderGraph& graph, std::vector<MeshUploadEntry>& meshUploads)
{
    auto& pass = graph.AddPass("MeshUpload", CommandType::Copy);

    for (auto& mesh : meshUploads)
        pass.writes.push_back({ mesh.handle, RGAccess::CopyDest });
    
    pass.gpuExecute = [this, meshUploads](CommandList& cmd, TaskContext& ctx) mutable {
        for (auto& mesh : meshUploads)
        {
            m_loader->UploadBufferRegion(cmd, ctx.upload->resource.Get(), mesh.region);
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
            auto& vb = ctx.GetResource<ComPtr<ID3D12Resource>>(mesh.hVb);
            auto& ib = ctx.GetResource<ComPtr<ID3D12Resource>>(mesh.hIb);

            auto vertexCount = static_cast<uint32_t>(mesh.asset->vertices.size());
            auto indexCount = static_cast<uint32_t>(mesh.asset->indices.size());

            auto vbHeapIndex = m_descFactory->CreateBufferSRV(vb.Get(), mesh.asset->vertexCount, mesh.asset->vertexStride);
            auto ibHeapIndex = m_descFactory->CreateBufferSRV(ib.Get(), indexCount, sizeof(uint32_t));

            m_registry->FinalizeMesh(
                mesh.hMesh.id,
                mesh.asset->format,
                vb, vbHeapIndex, vertexCount,
                ib, ibHeapIndex, indexCount);
        }
        };
}

RGHandle MeshGraphBuilder::CreateRGHandle()
{
    RGHandle handle{ m_nextId++ };
    return handle;
}