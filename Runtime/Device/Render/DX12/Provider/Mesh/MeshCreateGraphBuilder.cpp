#include "pch.h"
#include "MeshCreateGraphBuilder.h"
#include "MeshLoadRequest.h"
#include "MeshUtils.h"
#include "Resource/Mesh/StaticMeshResource.h"
#include "Core/Foundation/Align.h"
#include "Graph/TaskScheduler.h"
#include "Factory/DescriptorFactory.h"
#include "Factory/ResourceFactory.h"
#include "RenderConstants.h"

struct MeshUploadEntry
{
    RGResourceID vbResID{ InvalidRGID };
    RGResourceID ibResID{ InvalidRGID };

    UploadRegion vbRegion;
    UploadRegion ibRegion;

    std::shared_ptr<MeshResource> resource;
    std::shared_ptr<MeshAsset> asset;

    UINT vbHeapIndex{ UINT_MAX };
    UINT ibHeapIndex{ UINT_MAX };
};

MeshCreateGraphBuilder::~MeshCreateGraphBuilder() = default;
MeshCreateGraphBuilder::MeshCreateGraphBuilder(TaskScheduler& taskScheduler, ResourceFactory& resFactory,
    DescriptorFactory& descFactory) :
    m_taskScheduler{ taskScheduler },
    m_resFactory{ resFactory },
    m_descFactory{ descFactory }
{}

void MeshCreateGraphBuilder::LoadMeshes(const std::vector<MeshLoadRequest>& requests)
{
    m_idGenerator.Reset();
    m_graph.Reset();

    size_t totalUploadSize = 0;
    auto meshUploads = std::make_shared<std::vector<MeshUploadEntry>>(
        BuildMeshUploads(requests, totalUploadSize));

    RGResourceID uploadResID = m_idGenerator.Generate();

    for (const auto& mesh : *meshUploads)
    {
        m_graph.ImportResource(mesh.vbResID, RGAccess::CopyDest);
        m_graph.ImportResource(mesh.ibResID, RGAccess::CopyDest);
    }

    BuildUploadPass(meshUploads, uploadResID);

    for (const auto& mesh : *meshUploads)
    {
        m_graph.ExportResource(mesh.vbResID, RGAccess::SRV);
        m_graph.ExportResource(mesh.ibResID, RGAccess::SRV);
    }

    auto compiledTasks = m_graph.Compile();

    auto resContext = CreateResourceContext(meshUploads, uploadResID, totalUploadSize);
    m_taskScheduler.SubmitTask(compiledTasks, resContext);
}

std::vector<MeshUploadEntry> MeshCreateGraphBuilder::BuildMeshUploads(
    const std::vector<MeshLoadRequest>& requests,
    size_t& outTotalUploadSize)
{
    std::vector<MeshUploadEntry> uploads;
    uploads.reserve(requests.size());

    size_t offset = 0;
    for (const auto& req : requests)
    {
        RGResourceID vbResID = m_idGenerator.Generate();
        RGResourceID ibResID = m_idGenerator.Generate();

        auto vbRes = m_resFactory.CreateResource(static_cast<UINT64>(req.vbBytes), ResInitType::Default);
        auto ibRes = m_resFactory.CreateResource(static_cast<UINT64>(req.ibBytes), ResInitType::Default);

        // SRV는 실제 데이터 업로드 여부와 무관하게 만들 수 있으므로, 리소스 생성 직후 바로 구해둔다.
        auto indexCount = static_cast<UINT>(req.asset->indices.size());
        UINT vbHeapIndex = m_descFactory.CreateBufferSRV(
            DescriptorAllocationType::Persistent, vbRes, 0, req.asset->vertexCount, req.asset->vertexStride);
        UINT ibHeapIndex = m_descFactory.CreateBufferSRV(
            DescriptorAllocationType::Persistent, ibRes, 0, indexCount, sizeof(uint32_t));

        size_t vbOffset = offset;
        size_t ibOffset = offset + req.vbBytes;

        MeshUploadEntry entry;
        entry.vbResID = vbResID;
        entry.ibResID = ibResID;
        entry.vbRegion = { req.asset->vertices.data(), req.asset->vertices.size(), static_cast<UINT64>(vbOffset), vbRes };
        entry.ibRegion = { req.asset->indices.data(), req.asset->indices.size() * sizeof(uint32_t), static_cast<UINT64>(ibOffset), ibRes };
        entry.resource = req.resource;
        entry.asset = req.asset;
        entry.vbHeapIndex = vbHeapIndex;
        entry.ibHeapIndex = ibHeapIndex;

        uploads.push_back(std::move(entry));

        offset += req.vbBytes + req.ibBytes;
    }

    outTotalUploadSize = Core::AlignUp(offset, AlignVertexBuffer);
    return uploads;
}

std::shared_ptr<ResourceContext> MeshCreateGraphBuilder::CreateResourceContext(
    std::shared_ptr<std::vector<MeshUploadEntry>> meshUploads,
    RGResourceID uploadResID,
    size_t totalUploadSize)
{
    auto* rawContext = new ResourceContext();

    for (const auto& mesh : *meshUploads)
    {
        rawContext->Set(mesh.vbResID, mesh.vbRegion.dstBuffer);
        rawContext->Set(mesh.ibResID, mesh.ibRegion.dstBuffer);
    }
    rawContext->Set(uploadResID, m_resFactory.CreateResource(totalUploadSize, ResInitType::Upload));

    return std::shared_ptr<ResourceContext>(
        rawContext,
        [this, meshUploads](ResourceContext* ctx) mutable
        {
            FinalizeMeshes(*meshUploads);
            delete ctx;
        });
}

void MeshCreateGraphBuilder::BuildUploadPass(
    std::shared_ptr<std::vector<MeshUploadEntry>> meshUploads,
    RGResourceID uploadResID)
{
    auto& upload = m_graph.AddCopyPass("MeshUpload");

    for (auto& mesh : *meshUploads)
    {
        upload.Write(mesh.vbResID, RGAccess::CopyDest);
        upload.Write(mesh.ibResID, RGAccess::CopyDest);
    }

    upload.gpuExecute = [this, meshUploads, uploadResID](CommandList& cmd, TaskContext& ctx) mutable {
        auto& uploadRes = ctx.GetResource(uploadResID);
        for (auto& mesh : *meshUploads)
        {
            UploadBufferRegion(cmd, uploadRes, mesh.vbRegion);
            UploadBufferRegion(cmd, uploadRes, mesh.ibRegion);
        }
        };
}

void MeshCreateGraphBuilder::FinalizeMeshes(std::vector<MeshUploadEntry>& meshUploads)
{
    for (auto& mesh : meshUploads)
    {
        auto* meshRes = static_cast<StaticMeshResource*>(mesh.resource.get());

        auto vertexCount = static_cast<UINT>(mesh.asset->vertices.size());
        auto indexCount = static_cast<UINT>(mesh.asset->indices.size());

        meshRes->SetVertexFormat(mesh.asset->format);
        meshRes->SetResource(std::move(mesh.vbRegion.dstBuffer), std::move(mesh.ibRegion.dstBuffer), vertexCount, indexCount);
        meshRes->SetVertexHeapIndex(mesh.vbHeapIndex);
        meshRes->SetIndexHeapIndex(mesh.ibHeapIndex);
        meshRes->MarkReady();
    }
}