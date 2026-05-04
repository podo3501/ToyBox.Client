#include "pch.h"
#include "MeshGraphBuilder.h"
#include "RenderGraph.h"
#include "RenderPass.h"
#include "TaskScheduler.h"
#include "ResourceUploader.h"
#include "DescriptorFactory.h"
#include "MeshRegistry.h"
#include "GameClient/Service/Asset/Assets/MeshAsset.h"

MeshGraphBuilder::~MeshGraphBuilder() = default;
MeshGraphBuilder::MeshGraphBuilder(TaskScheduler* taskScheduler, ResourceUploader* uploader,
    DescriptorFactory* descriptorFactory, MeshRegistry* registry) :
    m_taskScheduler{ taskScheduler },
    m_uploader{ uploader },
    m_descriptorFactory{ descriptorFactory },
    m_registry{ registry }
{}

RGResource MeshGraphBuilder::LoadMesh(std::shared_ptr<MeshAsset> asset)
{
	RenderGraph graph;

    RGResource vb = graph.CreateResource();
    RGResource ib = graph.CreateResource();
	RGResource mesh = graph.CreateResource();
	
	BuildGraph(graph, asset, vb, ib, mesh);

    auto compiledTasks = graph.Compile(*m_taskScheduler);
    m_taskScheduler->Submit(compiledTasks, std::make_shared<ResourceContext>());

	return mesh;
}

void MeshGraphBuilder::BuildGraph(RenderGraph& graph, std::shared_ptr<MeshAsset> asset, 
    RGResource vbRes, RGResource ibRes, RGResource meshRes)
{
    auto& vbUpload = graph.AddPass("VBUpload", CommandType::Copy);
    vbUpload.writes.push_back({ vbRes, RGAccess::CopyDest });
    vbUpload.gpuExecute = [this, asset, vbRes](CommandList& cmd, TaskContext& ctx) {
        UploadBuffer uploadBuffer;
         auto resource = m_uploader->UploadVertexBuffer(cmd, asset->vertices, uploadBuffer);
        ctx.SetResource(vbRes, std::move(resource));
        ctx.SetPassContext(std::move(uploadBuffer));
        };

    auto& ibUpload = graph.AddPass("IBUpload", CommandType::Copy);
    ibUpload.writes.push_back({ ibRes, RGAccess::CopyDest });
    ibUpload.gpuExecute = [this, asset, ibRes](CommandList& cmd, TaskContext& ctx) {
        UploadBuffer uploadBuffer;
        auto resource = m_uploader->UploadIndexBuffer(cmd, asset->indices, uploadBuffer);
        ctx.SetResource(ibRes, std::move(resource));
        ctx.SetPassContext(std::move(uploadBuffer));
        };

    auto& finalize = graph.AddPass("FinalizeMesh", CommandType::None);
    finalize.reads.push_back({ vbRes, RGAccess::CopyDest });
    finalize.reads.push_back({ ibRes, RGAccess::CopyDest });
    finalize.writes.push_back({ vbRes, RGAccess::SRV });
    finalize.writes.push_back({ ibRes, RGAccess::SRV });
    finalize.cpuExecute = [this, asset, vbRes, ibRes, meshRes](TaskContext& ctx) {
        auto& vb = ctx.GetResource<ComPtr<ID3D12Resource>>(vbRes);
        auto& ib = ctx.GetResource<ComPtr<ID3D12Resource>>(ibRes);

        auto vertexCount = static_cast<uint32_t>(asset->vertices.size());
        auto indexCount = static_cast<uint32_t>(asset->indices.size());
        auto vbAlloc = m_descriptorFactory->CreateBufferSRV(vb.Get(), vertexCount, sizeof(Vertex));
        auto ibAlloc = m_descriptorFactory->CreateBufferSRV(ib.Get(), indexCount, sizeof(uint32_t));

        m_registry->FinalizeMesh(meshRes.id, vb, std::move(vbAlloc), ib, std::move(ibAlloc), indexCount);
        };
}