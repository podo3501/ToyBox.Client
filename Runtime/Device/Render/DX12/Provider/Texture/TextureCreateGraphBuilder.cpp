#include "pch.h"
#include "TextureCreateGraphBuilder.h"
#include "MipGenerator.h"
#include "Graph/RenderGraph.h"
#include "Graph/TaskScheduler.h"
#include "Factory/DescriptorFactory.h"
#include "Factory/ResourceFactory.h"
#include "TextureLoadRequest.h"
#include "Core/Foundation/Align.h"
#include "RenderConstants.h"
#include "TextureUtils.h"

struct TextureUploadEntry
{
    RGResourceID resID;
    Resource resource;
    std::shared_ptr<TextureAsset> asset;

    size_t offset{ 0 };
    bool generateMips{ false };
};

struct TextureFinalizeEntry
{
    RGResourceID resID;
    bool generateMips{ false };
};

TextureCreateGraphBuilder::~TextureCreateGraphBuilder() = default;
TextureCreateGraphBuilder::TextureCreateGraphBuilder(
    Device& device,
    TaskScheduler& taskScheduler, 
    ResourceFactory& resFactory,
    DescriptorFactory& descFactory) :
    m_mipGenerator{ device },
    m_taskScheduler{ taskScheduler },
    m_resFactory{ resFactory },
    m_descFactory{ descFactory }
{}

bool TextureCreateGraphBuilder::Initialize(ShaderLibrary& shaderLibrary)
{
    return m_mipGenerator.Initialize(shaderLibrary);
}

void TextureCreateGraphBuilder::LoadTextures(const std::vector<TextureLoadRequest>& requests)
{
    RenderGraph graph;

    std::vector<TextureUploadEntry> textureUploads;
    std::vector<TextureFinalizeEntry> finalizeEntries;

    size_t offset = 0;
    bool hasMipTask = false; //로딩하는 텍스쳐들중에 하나라도 mip생성이 있는지 확인
    for (const auto& req : requests)
    {
        RGResourceID texResID = RenderGraph::CreateRGResourceID();
        m_registry.Register(texResID, req.resource);

        auto& texDesc = req.resource->GetDesc();
        auto mips = CanGenerateMips(*req.asset, texDesc.generateMipmaps);
        auto resDesc = CreateTexture2DDesc(*req.asset, mips);
        auto texRes = m_resFactory.CreateTextureResource(resDesc);

        auto& textureResource = req.resource;
        textureResource->Set(texRes);
        textureResource->SetSize(req.asset->size);
        m_descFactory.CreateTextureViews(textureResource.get(), mips);

        hasMipTask |= mips;
        offset = Core::AlignUp(offset, AlignTexturePlacement);

        textureUploads.push_back({ texResID, texRes, req.asset, offset, mips });
        finalizeEntries.push_back({ texResID, mips });

        auto requiredSize = m_resFactory.GetRequiredIntermediateSize(resDesc, 0, 1, offset);
        offset += requiredSize;
    }
    RGResourceID uploadResID = RenderGraph::CreateRGResourceID();

    BuildUploadPass(graph, textureUploads, uploadResID);
    if (hasMipTask) BuildMipPass(graph, textureUploads); //하나라도 있으면 mip pass 생성을 함.
    BuildFinalizePass(graph, finalizeEntries);

    auto compiledTasks = graph.Compile();

    size_t totalUploadSize = Core::AlignUp(offset, AlignTexturePlacement);
    auto resCtx = std::make_shared<ResourceContext>();
    resCtx->Set(uploadResID, m_resFactory.CreateResource(totalUploadSize, ResInitType::Upload));

    m_taskScheduler.SubmitTask(compiledTasks, resCtx);
}

void TextureCreateGraphBuilder::BuildUploadPass(RenderGraph& graph, std::vector<TextureUploadEntry>& textureUploads, RGResourceID uploadResID)
{
    auto& upload = graph.AddCopyPass("TextureUpload");

    for (auto& tex : textureUploads)
        upload.Write(tex.resID, RGAccess::CopyDest);

    upload.gpuExecute = [this, textureUploads, uploadResID](CommandList& cmd, TaskContext& ctx) mutable {
        auto& uploadRes = ctx.GetResource(uploadResID);
        for (auto& tex : textureUploads)
        {
            UploadTexture(cmd, *tex.asset, tex.resource, uploadRes, tex.offset);
            ctx.SetResource(tex.resID, std::move(tex.resource));
        }
        };
}

void TextureCreateGraphBuilder::BuildMipPass(RenderGraph& graph, std::vector<TextureUploadEntry>& textureUploads)
{
    auto& mip = graph.AddComputePass("GenerateMips");

    for (auto& tex : textureUploads)
    {
        if (!tex.generateMips) continue; //개별적으로 mip을 할지 말지 여기서 결정

        mip.Write(tex.resID, RGAccess::UAV);
    }

    mip.gpuExecute = [this, textureUploads](CommandList& cmd, TaskContext& ctx) {
        for (auto& tex : textureUploads)
        {
            if (!tex.generateMips) continue; //개별적으로 mip을 할지 말지 여기서 결정

            auto texRes = m_registry.GetTextureResource(tex.resID);
            m_mipGenerator.GenerateMips(cmd, m_descFactory.GetBindlessAllocator(), texRes);
        }
        };
}

void TextureCreateGraphBuilder::BuildFinalizePass(RenderGraph& graph, std::vector<TextureFinalizeEntry>& finalizeEntries)
{
    auto& finalize = graph.AddCpuPass("FinalizeTexture");

    for (auto& tex : finalizeEntries)
        finalize.Read(tex.resID, RGAccess::SRV);

    finalize.cpuExecute = [this, finalizeEntries](TaskContext& ctx) {
        for (auto& tex : finalizeEntries)
            m_registry.FinalizeTexture(tex.resID);

        m_descFactory.GetBindlessAllocator().ResetAsyncTransient(); //mipmap때 임시로 만든 srv/uav 정리.
        };
}