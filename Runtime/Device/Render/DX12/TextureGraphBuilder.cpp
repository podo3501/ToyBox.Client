#include "pch.h"
#include "TextureGraphBuilder.h"
#include "RenderGraph.h"
#include "RenderPass.h"
#include "TextureRegistry.h"
#include "TaskScheduler.h"
#include "ResourceLoader.h"
#include "MipGenerator.h"
#include "DescriptorFactory.h"
#include "CommandUtils.h"
#include "CommandList.h"
#include "TextureLoadRequest.h"
#include "DX12Utils.h"

TextureGraphBuilder::~TextureGraphBuilder() = default;
TextureGraphBuilder::TextureGraphBuilder(TaskScheduler* taskScheduler, ResourceLoader* loader,
    MipGenerator* mipGenerator, DescriptorFactory* descriptorFactory) :
    m_taskScheduler{ taskScheduler },
    m_loader{ loader },
    m_mipGenerator{ mipGenerator },
    m_descriptorFactory{ descriptorFactory },
    m_registry{ make_unique<TextureRegistry>() }
{}

void TextureGraphBuilder::LoadTextures(const std::vector<TextureLoadRequest>& requests)
{
    RenderGraph graph;

    size_t offset = 0;
    for (const auto& req : requests)
    {
        RGHandle hTex = CreateRGHandle();
        m_registry->Register(hTex.id, req.resource);

        auto mips = m_loader->ShouldGenerateMips(*req.asset, req.desc.generateMips);
        auto texDesc = m_loader->CreateTexture2DDesc(*req.asset, mips);
        auto texRes = m_loader->CreateTextureResource(texDesc);

        offset = AlignSize(offset, AlignTexture);
        BuildGraph(graph, req.asset, req.desc, hTex, texRes, offset, mips);

        auto requiredSize = m_loader->GetTextureUploadLayout(texDesc, offset);
        offset += requiredSize;
    }

    auto compiledTasks = graph.Compile();
    
    size_t totalUploadSize = AlignSize(offset, AlignTexture);
    auto uploadCtx = std::make_shared<UploadContext>();
    uploadCtx->resource = m_loader->CreateUploadResource(totalUploadSize);
    m_taskScheduler->Submit(compiledTasks, std::make_shared<ResourceContext>(), uploadCtx);
}

void TextureGraphBuilder::BuildGraph(
    RenderGraph& graph, std::shared_ptr<TextureAsset> asset, const TextureDesc& desc, 
    RGHandle hTex, ComPtr<ID3D12Resource> texRes, size_t offset, bool generateMips)
{
    if (generateMips)
    {
        auto& mip = graph.AddPass("GenerateMips", CommandType::Compute);
        mip.reads.push_back({ hTex, RGAccess::CopyDest });
        mip.writes.push_back({ hTex, RGAccess::UAV });
        mip.gpuExecute = [this, hTex](CommandList& cmd, TaskContext& ctx) {
            auto& res = ctx.GetResource<ComPtr<ID3D12Resource>>(hTex);
            m_mipGenerator->GenerateMips(cmd, res.Get());
            };
    }
    
    auto& finalize = graph.AddPass("FinalizeTexture", CommandType::None);
    finalize.reads.push_back({ hTex, generateMips ? RGAccess::UAV : RGAccess::CopyDest});
    finalize.writes.push_back({ hTex, RGAccess::SRV });
    finalize.cpuExecute = [this, hTex, desc, generateMips](TaskContext& ctx) {
        auto& res = ctx.GetResource<ComPtr<ID3D12Resource>>(hTex);
        auto allocation = m_descriptorFactory->CreateTextureSRV(res.Get(), desc, generateMips);
        m_registry->FinalizeTexture(hTex.id, res, std::move(allocation));
        };

    auto& upload = graph.AddPass("TextureUpload", CommandType::Copy);
    upload.writes.push_back({ hTex, RGAccess::CopyDest});
    upload.gpuExecute = [this, asset, texRes, offset, hTex](CommandList& cmd, TaskContext& ctx) {
        m_loader->UploadTexture(cmd, *asset, texRes.Get(), ctx.upload->resource.Get(), offset);
        ctx.SetResource(hTex, std::move(texRes));
        };

    return;
}

RGHandle TextureGraphBuilder::CreateRGHandle()
{
    RGHandle handle{ m_nextId++ };
    return handle;
}