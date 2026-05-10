#include "pch.h"
#include "MeshGraphBuilder.h"
#include "RenderGraph.h"
#include "RenderPass.h"
#include "TaskScheduler.h"
#include "ResourceLoader.h"
#include "DescriptorFactory.h"
#include "MeshRegistry.h"
#include "MeshLoadRequest.h"
#include "DX12Utils.h"
#include "GameClient/Service/Asset/Assets/MeshAsset.h"

MeshGraphBuilder::~MeshGraphBuilder() = default;
MeshGraphBuilder::MeshGraphBuilder(TaskScheduler* taskScheduler, ResourceLoader* loader,
    DescriptorFactory* descriptorFactory) :
    m_taskScheduler{ taskScheduler },
    m_loader{ loader },
    m_descriptorFactory{ descriptorFactory },
    m_registry{ make_unique<MeshRegistry>() }
{}

void MeshGraphBuilder::LoadMeshes(const std::vector<MeshLoadRequest>& requests)
{
    RenderGraph graph;

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
        
        BuildGraph(graph, req.asset, hMesh, hVb, vbRes, hIb, ibRes, vbOffset, ibOffset);

        offset += req.vbBytes + req.ibBytes;
    }

    auto compiledTasks = graph.Compile();

    size_t totalUploadSize = AlignSize(offset, AlignVertexIndex);
    auto uploadCtx = std::make_shared<UploadContext>();
    uploadCtx->resource = m_loader->CreateUploadResource(totalUploadSize);
    m_taskScheduler->Submit(compiledTasks, std::make_shared<ResourceContext>(), uploadCtx);
}

void MeshGraphBuilder::BuildGraph(
    RenderGraph& graph, 
    std::shared_ptr<MeshAsset> asset, 
    RGHandle hMesh,
    RGHandle hVb, ComPtr<ID3D12Resource> vbRes, 
    RGHandle hIb, ComPtr<ID3D12Resource> ibRes, 
    size_t vbOffset, size_t ibOffset)
{
    auto& vbUpload = graph.AddPass("VBUpload", CommandType::Copy);
    vbUpload.writes.push_back({ hVb, RGAccess::CopyDest });
    vbUpload.gpuExecute = [this, asset, hVb, vbRes, vbOffset](CommandList& cmd, TaskContext& ctx) {
        UploadRegion vbRegion;
        vbRegion.data = asset->vertices.data();
        vbRegion.size = asset->vertices.size();
        vbRegion.srcOffset = static_cast<UINT64>(vbOffset);
        vbRegion.dstBuffer = vbRes.Get();

        m_loader->UploadBufferRegion(cmd, ctx.upload->resource.Get(), vbRegion);
        ctx.SetResource(hVb, std::move(vbRes));
        };

    auto& ibUpload = graph.AddPass("IBUpload", CommandType::Copy);
    ibUpload.writes.push_back({ hIb, RGAccess::CopyDest });
    ibUpload.gpuExecute = [this, asset, hIb, ibRes, ibOffset](CommandList& cmd, TaskContext& ctx) {
        UploadRegion ibRegion;
        ibRegion.data = asset->indices.data();
        ibRegion.size = asset->indices.size() * sizeof(uint32_t);
        ibRegion.srcOffset = static_cast<UINT64>(ibOffset);
        ibRegion.dstBuffer = ibRes.Get();

        m_loader->UploadBufferRegion(cmd, ctx.upload->resource.Get(), ibRegion);
        ctx.SetResource(hIb, std::move(ibRes));
        };

    auto& finalize = graph.AddPass("FinalizeMesh", CommandType::None);
    finalize.reads.push_back({ hVb, RGAccess::CopyDest });
    finalize.reads.push_back({ hIb, RGAccess::CopyDest });
    finalize.writes.push_back({ hVb, RGAccess::SRV });
    finalize.writes.push_back({ hIb, RGAccess::SRV });
    finalize.cpuExecute = [this, asset, hVb, hIb, hMesh](TaskContext& ctx) {
        auto& vb = ctx.GetResource<ComPtr<ID3D12Resource>>(hVb);
        auto& ib = ctx.GetResource<ComPtr<ID3D12Resource>>(hIb);

        auto indexCount = static_cast<uint32_t>(asset->indices.size());
        auto vbAlloc = m_descriptorFactory->CreateBufferSRV(vb.Get(), asset->vertexCount, asset->vertexStride);
        auto ibAlloc = m_descriptorFactory->CreateBufferSRV(ib.Get(), indexCount, sizeof(uint32_t));

        m_registry->FinalizeMesh(hMesh.id, vb, std::move(vbAlloc), ib, std::move(ibAlloc), indexCount);
        };
}

RGHandle MeshGraphBuilder::CreateRGHandle()
{
    RGHandle handle{ m_nextId++ };
    return handle;
}