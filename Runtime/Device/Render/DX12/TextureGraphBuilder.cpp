#include "pch.h"
#include "TextureGraphBuilder.h"
#include "RenderGraph.h"
#include "RenderPass.h"
#include "TextureRegistry.h"
#include "TaskScheduler.h"
#include "ResourceLoader.h"
#include "MipGenerator.h"
#include "Descriptor/DescriptorFactory.h"
#include "Descriptor/DescriptorAllocator.h"
#include "Helpers/CommandListHelpers.h"
#include "Command/CommandList.h"
#include "TextureLoadRequest.h"
#include "Helpers/CommonHelpers.h"

struct TextureUploadEntry
{
    RGHandle handle;
    ComPtr<ID3D12Resource> resource;
    std::shared_ptr<TextureAsset> asset;

    size_t offset{ 0 };
    bool generateMips{ false };
};

struct TextureFinalizeEntry
{
    RGHandle handle;
    bool generateMips{ false };
};

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

    std::vector<TextureUploadEntry> textureUploads;
    std::vector<TextureFinalizeEntry> finalizeEntries;

    size_t offset = 0;
    bool hasMipTask = false;
    for (const auto& req : requests)
    {
        RGHandle hTex = CreateRGHandle();
        m_registry->Register(hTex.id, req.resource);

        auto& texDesc = req.resource->GetDesc();
        auto mips = m_loader->ShouldGenerateMips(*req.asset, texDesc.generateMips);
        auto resDesc = m_loader->CreateTexture2DDesc(*req.asset, mips);
        auto texRes = m_loader->CreateTextureResource(resDesc);

        auto textureResource = std::static_pointer_cast<TextureResource>(req.resource);
        textureResource->SetResource(texRes);
        m_descriptorFactory->CreateTextureViews(textureResource.get(), mips);

        hasMipTask |= mips;
        offset = AlignSize(offset, AlignTexture);

        textureUploads.push_back({ hTex, texRes, req.asset, offset, mips });
        finalizeEntries.push_back({ hTex, mips });

        auto requiredSize = m_loader->GetTextureUploadLayout(resDesc, offset);
        offset += requiredSize;
    }

    BuildUploadPass(graph, textureUploads);
    if (hasMipTask) BuildMipPass(graph, textureUploads);
    BuildFinalizePass(graph, finalizeEntries);

    auto compiledTasks = graph.Compile();

    size_t totalUploadSize = AlignSize(offset, AlignTexture);
    auto uploadCtx = std::make_shared<UploadContext>();
    uploadCtx->resource = m_loader->CreateUploadResource(totalUploadSize);
    m_taskScheduler->Submit(compiledTasks, std::make_shared<ResourceContext>(), uploadCtx);
}

void TextureGraphBuilder::BuildUploadPass(RenderGraph& graph, std::vector<TextureUploadEntry>& textureUploads)
{
    auto& upload = graph.AddPass("TextureUpload", CommandType::Copy);

    for (auto& tex : textureUploads)
        upload.writes.push_back({ tex.handle, RGAccess::CopyDest });

    upload.gpuExecute = [this, textureUploads](CommandList& cmd, TaskContext& ctx) mutable {
        for (auto& tex : textureUploads)
        {
            m_loader->UploadTexture(cmd, *tex.asset, tex.resource.Get(), ctx.upload->resource.Get(), tex.offset);
            ctx.SetResource(tex.handle, std::move(tex.resource));
        }
        };
}

void TextureGraphBuilder::BuildMipPass(RenderGraph& graph, std::vector<TextureUploadEntry>& textureUploads)
{
    auto& mip = graph.AddPass("GenerateMips", CommandType::Compute);

    for (auto& tex : textureUploads)
    {
        if (!tex.generateMips) continue;

        mip.reads.push_back({ tex.handle, RGAccess::CopyDest });
        mip.writes.push_back({ tex.handle, RGAccess::UAV });
    }

    mip.gpuExecute = [this, textureUploads](CommandList& cmd, TaskContext& ctx) {
        for (auto& tex : textureUploads)
        {
            if (!tex.generateMips) continue;

            auto texRes = m_registry->GetTextureResource(tex.handle.id);
            m_mipGenerator->GenerateMips(cmd, texRes);
        }
        };
}

void TextureGraphBuilder::BuildFinalizePass(RenderGraph& graph, std::vector<TextureFinalizeEntry>& finalizeEntries)
{
    auto& finalize = graph.AddPass("FinalizeTexture", CommandType::None);

    for (auto& tex : finalizeEntries)
    {
        finalize.reads.push_back({ tex.handle, tex.generateMips ? RGAccess::UAV : RGAccess::CopyDest });
        finalize.writes.push_back({ tex.handle, RGAccess::SRV });
    }

    finalize.cpuExecute = [this, finalizeEntries](TaskContext& ctx) {
        for (auto& tex : finalizeEntries)
        {
            m_registry->FinalizeTexture(tex.handle.id);
        }
        m_descriptorFactory->GetDescriptorAllocator()->ResetTransient(); //mipmap때 임시로 만든 srv/uav 정리.
        };
}

RGHandle TextureGraphBuilder::CreateRGHandle()
{
    RGHandle handle{ m_nextId++ };
    return handle;
}