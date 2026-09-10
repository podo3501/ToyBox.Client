#include "pch.h"
#include "ForwardRenderPipeline.h"
#include "Graph/TaskUtils.h"
#include "Inspector/InspectorGraphBuilder.h"
#include "SwapChainPresenter.h"
#include "Factory/DescriptorFactory.h"
#include "Command/CommandList.h"
#include "Resource/Internal/ViewTargetResource.h"

ForwardRenderPipeline::~ForwardRenderPipeline() = default;
ForwardRenderPipeline::ForwardRenderPipeline(
    Device& device,
    SwapChainPresenter& swapChain,
    TaskScheduler& taskScheduler,
    DescriptorFactory& descFactory,
    ShaderLibrary& shaderLibaray,
    FontAtlasUploadGraphBuilder& fontUploadBuilder) :
    m_device{ device },
    m_swapChain{ swapChain },
    m_descFactory{ descFactory },
    m_renderers{ device, shaderLibaray },
    m_viewComposition{ device, taskScheduler, descFactory, m_renderers, swapChain, m_graph, m_idAllocator },
    m_inspectorRenderers{ device, shaderLibaray },
    //Builders
    m_fontUploadBuilder{ fontUploadBuilder },
    m_clearBuilder{ m_swapChain },
    m_shadowBuilder{
        m_renderers.GetShadowRenderer(),
        m_descFactory, m_shadowRes },
    m_inspectorBuilder{ m_inspectorRenderers.GetInspectorImageRenderer(),
        m_swapChain }
{}

bool ForwardRenderPipeline::Initialize(const Size& screenSize, const Size& shadowMapSize)
{
    ReturnIfFalse(m_shadowRes.Initialize(m_device, m_descFactory, shadowMapSize));
    ReturnIfFalse(m_renderers.Initialize());
    ReturnIfFalse(m_idAllocator.Initialize(TotalResourceIDCapacity, DynamicResourceIDCapacity));
    ReturnIfFalse(m_inspectorRenderers.Initialize(screenSize));

    m_hBackBuffer = m_idAllocator.AllocatePersistent();
    m_hShadow = m_idAllocator.AllocatePersistent();

    return true;
}

std::vector<CompiledTask> ForwardRenderPipeline::BuildFrame(const FramePacket& framePacket)
{
    m_renderers.ResetFrameResources(); // 이전 프레임에 썻던 데이터들을 초기화.
    m_graph.Reset();
    m_idAllocator.ResetTransient();

    m_graph.ImportResource(m_hBackBuffer, RGAccess::Present);
    m_graph.ImportResource(m_hShadow, RGAccess::DepthWrite);

    if (m_fontUploadBuilder.HasPendingUploads())
        m_fontUploadBuilder.Build(m_graph, m_idAllocator);

    m_clearBuilder.Build(m_graph, m_hBackBuffer);
    m_shadowBuilder.Build(m_graph, m_hShadow, framePacket.light, framePacket.shadowCasters);

    FramePassContext frameCtx{ framePacket.light, m_shadowRes, m_hShadow };
    m_viewComposition.Build(m_hBackBuffer, framePacket, frameCtx);

    //shadow map은 텍스쳐가 크기 때문에 작은 물체를 띄우면 안보인다. 
    //m_inspectorBuilder.Build(m_graph, m_hBackBuffer, 10); //인자는 보고싶은 srv Index(Heap Index)를 넣으면 된다. 

    m_graph.ExportResource(m_hBackBuffer, RGAccess::Present);
    m_graph.ExportResource(m_hShadow, RGAccess::DepthWrite);

    return m_graph.Compile();
}

void ForwardRenderPipeline::Update()
{
    m_viewComposition.Update();
}

CommandList* ForwardRenderPipeline::Render(
    CommandList* cmd, 
    CommandScheduler& cmdScheduler,
    FramePacket framePacket)
{
    auto compiledTasks = BuildFrame(framePacket); // 매 프레임 그래프 재구성

    TaskContext ctx;
    ctx.resources = std::make_shared<ResourceContext>(TotalResourceIDCapacity);

    m_fontUploadBuilder.ApplyResourceBindings(*ctx.resources);
    m_viewComposition.ApplyResourceBindings(*ctx.resources);
    ctx.SetResource(m_hBackBuffer, m_swapChain.GetCurrentBackbuffer());
    ctx.SetResource(m_hShadow, m_shadowRes.GetResource());

    return ExecuteRenderPipeline(cmd, cmdScheduler, compiledTasks, ctx);
}

void ForwardRenderPipeline::Resize(const Size& size)
{
    m_inspectorRenderers.SetScreenSize(size);
}