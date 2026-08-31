#include "pch.h"
#include "TextureCreateGraphBuilder.h"
#include "TextureLoadRequest.h"
#include "MipGenerator.h"
#include "TextureUtils.h"
#include "Resource/Texture/TextureResource.h"
#include "Core/Foundation/Align.h"
#include "Graph/TaskScheduler.h"
#include "Factory/DescriptorFactory.h"
#include "Factory/ResourceFactory.h"
#include "RenderConstants.h"

struct TextureUploadEntry
{
    RGResourceID resID;
    std::shared_ptr<TextureResource> resource;
    std::shared_ptr<TextureAsset> asset;

    size_t offset{ 0 };
    bool generateMips{ false };

    std::vector<UINT> mipSrvIndices;
    std::vector<UINT> mipUavIndices;
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
    m_idGenerator.Reset();
    m_graph.Reset();

    size_t totalUploadSize = 0;
    bool hasMipTask = false;
    auto textureUploads = std::make_shared<std::vector<TextureUploadEntry>>(
        BuildTextureUploads(requests, totalUploadSize, hasMipTask));

    RGResourceID uploadResID = m_idGenerator.Generate();

    // 텍스쳐는 RenderGraph가 트래킹하지 않는 persistent 리소스이므로
    // 이번 그래프 구간에서만 상태를 빌려와서(Import) 관리하고, 끝나면 SRV로 반환(Export)한다.
    for (const auto& upload : *textureUploads)
        m_graph.ImportResource(upload.resID, RGAccess::CopyDest);

    BuildUploadPass(textureUploads, uploadResID);
    if (hasMipTask) BuildMipPass(textureUploads); //하나라도 있으면 mip pass 생성을 함.

    for (const auto& upload : *textureUploads)
        m_graph.ExportResource(upload.resID, RGAccess::SRV);

    auto compiledTasks = m_graph.Compile();

    auto resContext = CreateResourceContext(textureUploads, uploadResID, totalUploadSize);
    m_taskScheduler.SubmitTask(compiledTasks, resContext);
}

std::vector<TextureUploadEntry> TextureCreateGraphBuilder::BuildTextureUploads(
    const std::vector<TextureLoadRequest>& requests,
    size_t& outTotalUploadSize,
    bool& outHasMipTask)
{
    std::vector<TextureUploadEntry> uploads;
    uploads.reserve(requests.size());

    size_t offset = 0;
    outHasMipTask = false; //로딩하는 텍스쳐들중에 하나라도 mip생성이 있는지 확인

    for (const auto& req : requests)
    {
        RGResourceID texResID = m_idGenerator.Generate();

        auto& texDesc = req.resource->GetDesc();
        auto mips = CanGenerateMips(*req.asset, texDesc.generateMipmaps);
        auto resDesc = CreateTexture2DDesc(*req.asset, mips);
        auto texRes = m_resFactory.CreateTextureResource(resDesc);

        auto& textureResource = req.resource;
        textureResource->Set(texRes);
        textureResource->SetSize(req.asset->size);

        std::vector<UINT> mipSrv, mipUav;
        m_descFactory.CreateTextureViews(
            textureResource.get(), mips,
            &mipSrv, &mipUav);

        outHasMipTask |= mips;
        offset = Core::AlignUp(offset, AlignTexturePlacement);
        uploads.push_back({
            texResID, textureResource, req.asset, offset, mips, 
            std::move(mipSrv), std::move(mipUav) });

        offset += m_resFactory.GetRequiredIntermediateSize(resDesc, 0, 1, offset);
    }

    outTotalUploadSize = Core::AlignUp(offset, AlignTexturePlacement);
    return uploads;
}

std::shared_ptr<ResourceContext> TextureCreateGraphBuilder::CreateResourceContext(
    std::shared_ptr<std::vector<TextureUploadEntry>> textureUploads,
    RGResourceID uploadResID,
    size_t totalUploadSize)
{
    auto* rawContext = new ResourceContext();
    
    for (const auto& upload : *textureUploads)
        rawContext->Set(upload.resID, upload.resource->Get());
    rawContext->Set(uploadResID, m_resFactory.CreateResource(totalUploadSize, ResInitType::Upload));

    // resContext가 소멸될 때(= GPU 작업 완료 후 TaskScheduler가 참조를 놓을 때) finalize 수행.
    return std::shared_ptr<ResourceContext>(
        rawContext,
        [this, textureUploads](ResourceContext* ctx) mutable
        {
            FinalizeTextures(*textureUploads);
            delete ctx;
        });
}

void TextureCreateGraphBuilder::BuildUploadPass(
    std::shared_ptr<std::vector<TextureUploadEntry>> textureUploads,
    RGResourceID uploadResID)
{
    auto& upload = m_graph.AddCopyPass("TextureUpload");

    for (auto& tex : *textureUploads)
        upload.Write(tex.resID, RGAccess::CopyDest);

    upload.gpuExecute = [this, textureUploads, uploadResID](CommandList& cmd, TaskContext& ctx) mutable {
        auto& uploadRes = ctx.GetResource(uploadResID);
        for (auto& upload : *textureUploads)
            UploadTexture(cmd, *upload.asset, upload.resource->Get(), uploadRes, upload.offset);
        };
}

void TextureCreateGraphBuilder::BuildMipPass(
    std::shared_ptr<std::vector<TextureUploadEntry>> textureUploads)
{
    auto& mip = m_graph.AddComputePass("GenerateMips");

    for (auto& tex : *textureUploads)
    {
        if (!tex.generateMips) continue; //개별적으로 mip을 할지 말지 여기서 결정
        mip.Write(tex.resID, RGAccess::UAV);
    }

    mip.gpuExecute = [this, textureUploads](CommandList& cmd, TaskContext& ctx) {
        for (auto& tex : *textureUploads)
        {
            if (!tex.generateMips) continue; //개별적으로 mip을 할지 말지 여기서 결정

            m_mipGenerator.GenerateMips(
                cmd, 
                m_descFactory.GetBindlessAllocator(), 
                tex.resource.get(),
                tex.mipSrvIndices,
                tex.mipUavIndices);
        }
        };
}

void TextureCreateGraphBuilder::FinalizeTextures(std::vector<TextureUploadEntry>& textureUploads)
{
    auto& allocator = m_descFactory.GetBindlessAllocator();

    for (auto& tex : textureUploads)
    {
        for (UINT srvIndex : tex.mipSrvIndices)
            allocator.FreeDynamic(srvIndex);
        for (UINT uavIndex : tex.mipUavIndices)
            allocator.FreeDynamic(uavIndex);

        tex.resource->MarkReady();
    }
}