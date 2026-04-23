#include "pch.h"
#include "TextureGraphBuilder.h"
#include "RenderGraph.h"
#include "RenderPass.h"
#include "TaskScheduler.h"
#include "ResourceUploader.h"
#include "MipGenerator.h"
#include "DescriptorFactory.h"
#include "CommandUtils.h"
#include "CommandList.h"

TextureGraphBuilder::~TextureGraphBuilder() = default;
TextureGraphBuilder::TextureGraphBuilder(TaskScheduler* taskScheduler, ResourceUploader* uploader,
    MipGenerator* mipGenerator, DescriptorFactory* descriptorFactory, TextureRegistry* registry) :
    m_taskScheduler{ taskScheduler },
    m_uploader{ uploader },
    m_mipGenerator{ mipGenerator },
    m_descriptorFactory{ descriptorFactory },
    m_registry{ registry }
{}

RGTexture TextureGraphBuilder::LoadTexture(std::shared_ptr<TextureAsset> asset, const TextureDesc& desc)
{
    RenderGraph graph;
    RGTexture tex = graph.CreateTexture(desc);

    BuildTextureGraph(graph, asset, desc, tex);

    graph.Compile(*m_taskScheduler);
    m_taskScheduler->Execute();

    return tex;
}

void TextureGraphBuilder::BuildTextureGraph(RenderGraph& graph, std::shared_ptr<TextureAsset> asset,
    const TextureDesc& desc, RGTexture tex)
{
    bool generateMips = m_uploader->ShouldGenerateMips(*asset, desc.generateMips);

    // 1. Upload Pass
    auto& upload = graph.AddPass("TextureUpload", CommandType::Copy);
    upload.writes.push_back(tex);
    upload.execute = [this, asset, generateMips, tex](CommandList& cmd, TaskContext& ctx) {
        auto& uploadCtx = std::get<UploadContext>(ctx.passData);
        auto resource = m_uploader->UploadTexture(cmd, *asset, generateMips, uploadCtx.uploadBuffer);
        ctx.resources->textures[tex.id] = resource;
        };

    // 2. Common -> SRV
    auto& toSrv = graph.AddPass("ToSRV", CommandType::Direct);
    toSrv.reads.push_back(tex);
    toSrv.writes.push_back(tex);
    toSrv.execute = [this, tex](CommandList& cmd, TaskContext& ctx) {
        auto& res = ctx.resources->textures[tex.id];
        CommandUtils::Transition(cmd, res.Get(),
            D3D12_RESOURCE_STATE_COMMON,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        };

    auto& createSrv = graph.AddPass("CreateSRV", CommandType::Direct);
    createSrv.reads.push_back(tex);
    createSrv.execute = [this, tex, desc, generateMips](CommandList&, TaskContext& ctx) {
        auto& res = ctx.resources->textures[tex.id];
        auto allocation = m_descriptorFactory->CreateSRV(res.Get(), desc, generateMips);
        ctx.resources->srvAllocations[tex.id] = std::move(allocation);
        };

    if (!generateMips)
    {
        createSrv.onComplete = [this, tex](TaskContext& ctx) {
            auto data = ctx.resources->TakeTexture(tex.id);
            m_registry->FinalizeTexture(tex.id, std::move(data.resource), std::move(data.srv));
            };
        return;
    }
        
    // 3. Mip chain (optional)    
    auto& toUav = graph.AddPass("ToUAV", CommandType::Direct);
    toUav.reads.push_back(tex);
    toUav.writes.push_back(tex);
    toUav.execute = [this, tex](CommandList& cmd, TaskContext& ctx) {
        auto& res = ctx.resources->textures[tex.id];
        CommandUtils::Transition(cmd, res.Get(),
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
            D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
        };

    auto& mip = graph.AddPass("GenerateMips", CommandType::Compute);
    mip.reads.push_back(tex);
    mip.writes.push_back(tex);
    mip.execute = [this, tex](CommandList& cmd, TaskContext& ctx) {
        auto& res = ctx.resources->textures[tex.id];
        m_mipGenerator->GenerateMips(cmd, res.Get());
        };

    auto& finalize = graph.AddPass("FinalizeSRV", CommandType::Direct);
    finalize.reads.push_back(tex);
    finalize.writes.push_back(tex);
    finalize.execute = [this, tex](CommandList& cmd, TaskContext& ctx) {
        auto& res = ctx.resources->textures[tex.id];
        CommandUtils::Transition(cmd, res.Get(),
            D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        };
    finalize.onComplete = [this, tex](TaskContext& ctx) {
        auto data = ctx.resources->TakeTexture(tex.id);
        m_registry->FinalizeTexture(tex.id, std::move(data.resource), std::move(data.srv));
        };
}