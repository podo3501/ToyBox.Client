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
    auto resources = std::make_shared<ResourceContext>();

    BuildTextureGraph(graph, asset, desc, tex); //?!? graph를 주면 texture graph가 pass를 만들어서 주는식인데 이게 TextureGraphBuiler 안에서 이루어 지는게 아니라 이 위에 클래스에서 해야할 것 같다.

    auto compiledTasks = graph.Compile(*m_taskScheduler);
    m_taskScheduler->Submit(compiledTasks, resources);

    return tex;
}

void TextureGraphBuilder::BuildTextureGraph(RenderGraph& graph, std::shared_ptr<TextureAsset> asset,
    const TextureDesc& desc, RGTexture tex)
{
    bool generateMips = m_uploader->ShouldGenerateMips(*asset, desc.generateMips);

    if (generateMips)
    {
        auto& mip = graph.AddPass("GenerateMips", CommandType::Compute);
        mip.accesses.push_back({ tex,
            ResourceState::TransferWrite,
            ResourceState::TransferWrite,
            ResourceAccessType::UAV
            });

        mip.reads.push_back({ tex, RGAccess::UAV });
        mip.writes.push_back({ tex, RGAccess::UAV });

        mip.gpuExecute = [this, tex](CommandList& cmd, TaskContext& ctx) {
            auto& res = ctx.GetTexture(tex);
            m_mipGenerator->GenerateMips(cmd, res.Get());
            };
    }

    auto& createSrv = graph.AddPass("CreateSRV", CommandType::None);
    createSrv.accesses.push_back({ tex,
        ResourceState::TransferWrite,
        ResourceState::ShaderRead,
        ResourceAccessType::SRV
        });

    createSrv.reads.push_back({ tex, RGAccess::SRV });

    createSrv.cpuExecute = [this, tex, desc, generateMips](TaskContext& ctx) {
        auto& res = ctx.GetTexture(tex);
        auto allocation = m_descriptorFactory->CreateSRV(res.Get(), desc, generateMips);
        ctx.SetSRV(tex, std::move(allocation));
        };

    auto& finalize = graph.AddPass("FinalizeTexture", CommandType::None);
    finalize.accesses.push_back({ tex,
        ResourceState::ShaderRead,
        ResourceState::ShaderRead,
        ResourceAccessType::SRV
        });

    finalize.reads.push_back({ tex, RGAccess::SRV });

    finalize.cpuExecute = [this, tex](TaskContext& ctx) {
        auto data = ctx.resources->TakeTexture(tex.id);
        m_registry->FinalizeTexture(tex.id, std::move(data.resource), std::move(data.srv));
        };


    auto& upload = graph.AddPass("TextureUpload", CommandType::Copy);
    upload.accesses.push_back({ tex,
        ResourceState::Undefined,
        ResourceState::TransferWrite,
        ResourceAccessType::Copy
        });

    upload.writes.push_back({ tex, RGAccess::Write });

    upload.gpuExecute = [this, asset, generateMips, tex](CommandList& cmd, TaskContext& ctx) {
        auto& uploadCtx = std::get<UploadContext>(ctx.passData);
        auto resource = m_uploader->UploadTexture(cmd, *asset, generateMips, uploadCtx.uploadBuffer);
        ctx.SetTexture(tex, std::move(resource));
        };


    return;
}