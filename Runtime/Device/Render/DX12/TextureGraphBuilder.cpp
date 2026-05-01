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

RGResource TextureGraphBuilder::LoadTexture(std::shared_ptr<TextureAsset> asset, const TextureDesc& desc)
{
    RenderGraph graph;
    RGResource tex = graph.CreateResource();
    auto resources = std::make_shared<ResourceContext>();

    BuildGraph(graph, asset, desc, tex); //?!? graph를 주면 texture graph가 pass를 만들어서 주는식인데 이게 TextureGraphBuiler 안에서 이루어 지는게 아니라 이 위에 클래스에서 해야할 것 같다.

    auto compiledTasks = graph.Compile(*m_taskScheduler);
    m_taskScheduler->Submit(compiledTasks, resources);

    return tex;
}

void TextureGraphBuilder::BuildGraph(RenderGraph& graph, std::shared_ptr<TextureAsset> asset,
    const TextureDesc& desc, RGResource texRes)
{
    bool generateMips = m_uploader->ShouldGenerateMips(*asset, desc.generateMips);

    if (generateMips)
    {
        auto& mip = graph.AddPass("GenerateMips", CommandType::Compute);
        mip.reads.push_back({ texRes, RGAccess::CopyDest });
        mip.writes.push_back({ texRes, RGAccess::UAV });
        mip.gpuExecute = [this, texRes](CommandList& cmd, TaskContext& ctx) {
            auto& res = ctx.GetResource(texRes);
            m_mipGenerator->GenerateMips(cmd, res.Get());
            };
    }

    auto& createSrv = graph.AddPass("CreateSRV", CommandType::None);
    createSrv.reads.push_back({ texRes, generateMips ? RGAccess::UAV : RGAccess::CopyDest});
    createSrv.writes.push_back({ texRes, RGAccess::SRV });
    createSrv.cpuExecute = [this, texRes, desc, generateMips](TaskContext& ctx) {
        auto& res = ctx.GetResource(texRes);
        auto allocation = m_descriptorFactory->CreateSRV(res.Get(), desc, generateMips);
        m_registry->FinalizeTexture(texRes.id, res, std::move(allocation));
        };

    auto& upload = graph.AddPass("TextureUpload", CommandType::Copy);
    upload.writes.push_back({ texRes, RGAccess::CopyDest});
    upload.gpuExecute = [this, asset, generateMips, texRes](CommandList& cmd, TaskContext& ctx) {
        auto& uploadCtx = std::get<UploadContext>(ctx.passData);
        auto resource = m_uploader->UploadTexture(cmd, *asset, generateMips, uploadCtx.uploadBuffer);
        ctx.SetResource(texRes, std::move(resource));
        };

    return;
}