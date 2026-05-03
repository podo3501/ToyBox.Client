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
	RGResource mesh = graph.CreateResource();
	auto resources = std::make_shared<ResourceContext>();

	BuildGraph(graph, asset, mesh);

    auto compiledTasks = graph.Compile(*m_taskScheduler);
    m_taskScheduler->Submit(compiledTasks, resources);

	return mesh;
}

void MeshGraphBuilder::BuildGraph(RenderGraph& graph, std::shared_ptr<MeshAsset> asset, RGResource meshRes)
{
    auto& vbUpload = graph.AddPass("VBUpload", CommandType::Copy);
    vbUpload.writes.push_back({ meshRes, RGAccess::CopyDest });
    vbUpload.gpuExecute = [this, asset, meshRes](CommandList& cmd, TaskContext& ctx) {
        UploadBuffer uploadBuffer;
         auto resource = m_uploader->UploadVertexBuffer(cmd, asset->vertices, uploadBuffer);
        ctx.SetResource(meshRes, std::move(resource));
        ctx.SetPassContext(std::move(uploadBuffer));
        };

    auto& ibUpload = graph.AddPass("IBUpload", CommandType::Copy);
    ibUpload.writes.push_back({ meshRes, RGAccess::CopyDest });
    ibUpload.gpuExecute = [this, asset, meshRes](CommandList& cmd, TaskContext& ctx) {
        UploadBuffer uploadBuffer;
        auto resource = m_uploader->UploadIndexBuffer(cmd, asset->indices, uploadBuffer);
        ctx.SetResource(meshRes, std::move(resource));
        ctx.SetPassContext(std::move(uploadBuffer));
        };

    auto& finalize = graph.AddPass("FinalizeMesh", CommandType::None);
    // 두 업로드 패스가 모두 완료된 후 실행되도록 의존성 연결
    finalize.reads.push_back({ meshRes, RGAccess::CopyDest });
    finalize.writes.push_back({ meshRes, RGAccess::SRV });

    //finalize.cpuExecute = [this, asset, meshRes](TaskContext& ctx) {
    //    auto& res = ctx.GetResource<ComPtr<ID3D12Resource>>(meshRes);

    //    auto vb = ctx.GetResource<Microsoft::WRL::ComPtr<ID3D12Resource>>("TempVB");
    //    auto ib = ctx.GetResource<Microsoft::WRL::ComPtr<ID3D12Resource>>("TempIB");

    //    MeshBundle bundle{ vb, ib };

    //    auto indexCount = static_cast<uint32_t>(asset->indices.size());
    //    auto vbAlloc = m_descriptorFactory->CreateBufferSRV(bundle.vb.Get(),
    //        static_cast<uint32_t>(asset->vertices.size()), sizeof(MeshAsset::Vertex));
    //    auto ibAlloc = m_descriptorFactory->CreateBufferSRV(bundle.ib.Get(), indexCount, sizeof(uint32_t));

    //    // 최종적으로 Registry에 등록
    //    m_registry->FinalizeMesh(meshRes.id, bundle.vb, std::move(vbAlloc), bundle.ib, std::move(ibAlloc), indexCount);

    //    // 최종 번들을 리소스 핸들에 할당
    //    ctx.SetResource(meshRes, std::move(bundle));
    //    };
}



















//void MeshGraphBuilder::BuildGraph(RenderGraph& graph, std::shared_ptr<MeshAsset> asset,
//	RGResource meshRes)
//{
//    auto& upload = graph.AddPass("MeshUpload", CommandType::Copy);
//    upload.writes.push_back({ meshRes, RGAccess::CopyDest });
//    upload.gpuExecute = [this, asset, meshRes](CommandList& cmd, TaskContext& ctx) {
//        UploadBuffer uploadBuffer;
//        auto resource = m_uploader->UploadMesh(cmd, *asset, uploadBuffer);
//        ctx.SetResource(meshRes, std::move(resource));
//        ctx.SetPassContext(std::move(uploadBuffer));
//        };
//
//    auto& finalize = graph.AddPass("FinalizeMesh", CommandType::None);
//    finalize.reads.push_back({ meshRes, RGAccess::CopyDest });
//    finalize.writes.push_back({ meshRes, RGAccess::SRV });
//    finalize.cpuExecute = [this, asset, meshRes](TaskContext& ctx) {
//        auto& res = ctx.GetResource<MeshBundle>(meshRes);
//        auto& vbRes = res.vb;
//        auto& ibRes = res.ib;
//        auto indexCount = static_cast<uint32_t>(asset->indices.size());
//        auto vbAlloc = m_descriptorFactory->CreateBufferSRV(vbRes.Get(),
//            static_cast<uint32_t>(asset->vertices.size()), sizeof(MeshAsset::Vertex));
//        auto ibAlloc = m_descriptorFactory->CreateBufferSRV(ibRes.Get(), indexCount, sizeof(uint32_t));
//        m_registry->FinalizeMesh(meshRes.id, vbRes, std::move(vbAlloc), ibRes, std::move(ibAlloc), indexCount);
//        };
//}