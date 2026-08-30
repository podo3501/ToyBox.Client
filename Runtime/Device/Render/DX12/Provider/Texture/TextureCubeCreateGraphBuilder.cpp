#include "pch.h"
#include "TextureCubeCreateGraphBuilder.h"
#include "Graph/TaskScheduler.h"
#include "Factory/DescriptorFactory.h"
#include "Factory/ResourceFactory.h"
#include "TextureCubeLoadRequest.h"
#include "TextureUtils.h"
#include "Core/Foundation/Align.h"
#include "RenderConstants.h"

struct TextureCubeUploadEntry
{
    RGResourceID resID;
    Resource resource;
    std::shared_ptr<TextureCubeAsset> asset;
    size_t offset{ 0 };
};

struct TextureCubeFinalizeEntry
{
    RGResourceID resID;
};

TextureCubeCreateGraphBuilder::~TextureCubeCreateGraphBuilder() = default;
TextureCubeCreateGraphBuilder::TextureCubeCreateGraphBuilder(
    TaskScheduler& taskScheduler,
    ResourceFactory& resFactory,
    DescriptorFactory& descFactory) :
    m_taskScheduler{ taskScheduler },
    m_resFactory{ resFactory },
    m_descFactory{ descFactory }
{}

void TextureCubeCreateGraphBuilder::LoadTextureCubes(const std::vector<TextureCubeLoadRequest>& requests)
{
    m_graph.Reset();

    std::vector<TextureCubeUploadEntry> uploads;
    std::vector<TextureCubeFinalizeEntry> finalizeEntries;

    size_t offset = 0;
    for (const auto& req : requests)
    {
        RGResourceID texResID = m_idGenerator.Generate();
        m_registry.Register(texResID, req.resource);

        auto resDesc = CreateTextureCubeDesc(*req.asset);
        auto texRes = m_resFactory.CreateTextureResource(resDesc);

        req.resource->Set(texRes);
        req.resource->SetSize(Size{ req.asset->width, req.asset->height });
        m_descFactory.CreateTextureCubeViews(req.resource.get()); // TextureCube SRV 생성 - 새로 추가 필요

        offset = Core::AlignUp(offset, AlignTexturePlacement);
        uploads.push_back({ texResID, texRes, req.asset, offset });
        finalizeEntries.push_back({ texResID });

        // 6면 * mipCount 개의 subresource 크기 합산
        auto requiredSize = m_resFactory.GetRequiredIntermediateSize(
            resDesc, 0, req.asset->mipCount * req.asset->faceCount, offset);
        offset += requiredSize;
    }
    RGResourceID uploadResID = m_idGenerator.Generate();

    BuildUploadPass(uploads, uploadResID);
    BuildFinalizePass(finalizeEntries);

    auto compiledTasks = m_graph.Compile();

    size_t totalUploadSize = Core::AlignUp(offset, AlignTexturePlacement);
    auto resCtx = std::make_shared<ResourceContext>();
    resCtx->Set(uploadResID, m_resFactory.CreateResource(totalUploadSize, ResInitType::Upload));

    m_taskScheduler.SubmitTask(compiledTasks, resCtx);
}

void TextureCubeCreateGraphBuilder::BuildUploadPass(std::vector<TextureCubeUploadEntry>& uploads, RGResourceID uploadResID)
{
    auto& upload = m_graph.AddCopyPass("TextureCubeUpload");

    for (auto& tex : uploads)
        upload.Write(tex.resID, RGAccess::CopyDest);

    upload.gpuExecute = [this, uploads, uploadResID](CommandList& cmd, TaskContext& ctx) mutable {
        auto& uploadRes = ctx.GetResource(uploadResID);
        for (auto& tex : uploads)
        {
            // subImages[mip + face * mipCount] 순서로 6면*N밉 전부 CopyTextureRegion
            UploadTextureCube(cmd, *tex.asset, tex.resource, uploadRes, tex.offset);
            ctx.SetResource(tex.resID, std::move(tex.resource));
        }
        };
}

void TextureCubeCreateGraphBuilder::BuildFinalizePass(std::vector<TextureCubeFinalizeEntry>& finalizeEntries)
{
    auto& finalize = m_graph.AddCpuPass("FinalizeTextureCube");

    for (auto& tex : finalizeEntries)
        finalize.Read(tex.resID, RGAccess::SRV);

    finalize.cpuExecute = [this, finalizeEntries](TaskContext& ctx) {
        for (auto& tex : finalizeEntries)
            m_registry.FinalizeTextureCube(tex.resID);
        };
}