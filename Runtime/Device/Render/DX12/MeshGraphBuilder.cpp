#include "pch.h"
#include "MeshGraphBuilder.h"
#include "RenderGraph.h"
#include "RenderPass.h"
#include "TaskScheduler.h"
#include "ResourceUploader.h"

MeshGraphBuilder::~MeshGraphBuilder() = default;
MeshGraphBuilder::MeshGraphBuilder(TaskScheduler* taskScheduler, ResourceUploader* uploader) :
    m_taskScheduler{ taskScheduler },
    m_uploader{ uploader }
{}

RGResource MeshGraphBuilder::LoadMesh(std::shared_ptr<MeshAsset> asset)
{
	RenderGraph graph;
	RGResource mesh = graph.CreateResource();
	auto resources = std::make_shared<ResourceContext>();

	BuildGraph(graph, asset, mesh);

	return mesh;
}

void MeshGraphBuilder::BuildGraph(RenderGraph& graph, std::shared_ptr<MeshAsset> asset,
	RGResource meshRes)
{
    auto& upload = graph.AddPass("MeshUpload", CommandType::Copy);
    upload.writes.push_back({ meshRes, RGAccess::CopyDest });
    upload.gpuExecute = [this, asset, meshRes](CommandList& cmd, TaskContext& ctx) {
        UploadBuffer uploadBuffer;
        auto resource = m_uploader->UploadMesh(cmd, *asset, uploadBuffer);
        ctx.SetResource(meshRes, std::move(resource));
        ctx.SetPassContext(std::move(uploadBuffer));
        };

    auto& finalize = graph.AddPass("FinalizeMesh", CommandType::None);
    finalize.reads.push_back({ meshRes, RGAccess::CopyDest });
    finalize.writes.push_back({ meshRes, RGAccess::SRV });
    finalize.cpuExecute = [this, meshRes](TaskContext& ctx) {
        //auto& res = ctx.GetResource<ComPtr<ID3D12Resource>>(meshRes);
        //auto allocation = m_descriptorFactory->CreateMeshViews(res.Get());
        //m_registry->FinalizeMesh(meshRes.id, res, std::move(allocation));
        };
}