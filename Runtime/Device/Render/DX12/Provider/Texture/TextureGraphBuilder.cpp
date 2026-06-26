#include "pch.h"
#include "TextureGraphBuilder.h"
#include "MipGenerator.h"
#include "Graph/RenderGraph.h"
#include "Graph/TaskScheduler.h"
#include "Factory/DescriptorFactory.h"
#include "Factory/ResourceFactory.h"
#include "TextureLoadRequest.h"
#include "Helpers/CommonHelpers.h"
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

TextureGraphBuilder::~TextureGraphBuilder() = default;
TextureGraphBuilder::TextureGraphBuilder(TaskScheduler& taskScheduler, ResourceFactory& resFactory,
    MipGenerator& mipGenerator, DescriptorFactory& descFactory) :
    m_taskScheduler{ taskScheduler },
    m_resFactory{ resFactory },
    m_mipGenerator{ mipGenerator },
    m_descFactory{ descFactory }
{}

void TextureGraphBuilder::LoadTextures(const std::vector<TextureLoadRequest>& requests)
{
    RenderGraph graph;

    std::vector<TextureUploadEntry> textureUploads;
    std::vector<TextureFinalizeEntry> finalizeEntries;

    size_t offset = 0;
    bool hasMipTask = false;
    for (const auto& req : requests)
    {
        RGResourceID texResID = RenderGraph::CreateRGResourceID();
        m_registry.Register(texResID, req.resource);

        auto& texDesc = req.resource->GetDesc();
        auto mips = CanGenerateMips(*req.asset, texDesc.generateMips);
        auto resDesc = CreateTexture2DDesc(*req.asset, mips);
        auto texRes = m_resFactory.CreateTextureResource(resDesc);

        auto textureResource = std::static_pointer_cast<TextureResource>(req.resource);
        textureResource->Set(texRes);
        m_descFactory.CreateTextureViews(textureResource.get(), mips);

        hasMipTask |= mips;
        offset = AlignSize(offset, AlignTexture);

        textureUploads.push_back({ texResID, texRes, req.asset, offset, mips });
        finalizeEntries.push_back({ texResID, mips });

        auto requiredSize = m_resFactory.GetRequiredIntermediateSize(resDesc, 0, 1, offset);
        offset += requiredSize;
    }
    RGResourceID uploadResID = RenderGraph::CreateRGResourceID();

    BuildUploadPass(graph, textureUploads, uploadResID);
    if (hasMipTask) BuildMipPass(graph, textureUploads);
    BuildFinalizePass(graph, finalizeEntries);

    auto compiledTasks = graph.Compile();

    size_t totalUploadSize = AlignSize(offset, AlignTexture);
    auto resCtx = std::make_shared<ResourceContext>();
    resCtx->Set(uploadResID, m_resFactory.CreateResource(totalUploadSize, ResInitType::Upload));

    m_taskScheduler.Submit(compiledTasks, resCtx);
}

void TextureGraphBuilder::BuildUploadPass(RenderGraph& graph, std::vector<TextureUploadEntry>& textureUploads, RGResourceID uploadResID)
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

void TextureGraphBuilder::BuildMipPass(RenderGraph& graph, std::vector<TextureUploadEntry>& textureUploads)
{
    auto& mip = graph.AddComputePass("GenerateMips");

    for (auto& tex : textureUploads)
        mip.Write(tex.resID, RGAccess::UAV); 

    mip.gpuExecute = [this, textureUploads](CommandList& cmd, TaskContext& ctx) {
        for (auto& tex : textureUploads)
        {
            if (!tex.generateMips) continue;

            auto texRes = m_registry.GetTextureResource(tex.resID);
            m_mipGenerator.GenerateMips(cmd, m_descFactory.GetBindlessAllocator(), texRes);
        }
        };
}

void TextureGraphBuilder::BuildFinalizePass(RenderGraph& graph, std::vector<TextureFinalizeEntry>& finalizeEntries)
{
    auto& finalize = graph.AddCpuPass("FinalizeTexture");

    for (auto& tex : finalizeEntries)
        finalize.Read(tex.resID, RGAccess::SRV);

    finalize.cpuExecute = [this, finalizeEntries](TaskContext& ctx) {
        for (auto& tex : finalizeEntries)
            m_registry.FinalizeTexture(tex.resID);

        m_descFactory.GetBindlessAllocator().ResetTransient(); //mipmap때 임시로 만든 srv/uav 정리.
        };
}