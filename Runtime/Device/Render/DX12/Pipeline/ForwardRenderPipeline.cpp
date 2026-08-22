#include "pch.h"
#include "ForwardRenderPipeline.h"
#include "Graph/TaskUtils.h"
#include "Inspector/InspectorGraphBuilder.h"
#include "SwapChainPresenter.h"
#include "Factory/DescriptorFactory.h"
#include "Command/CommandList.h"

ForwardRenderPipeline::~ForwardRenderPipeline() = default;
ForwardRenderPipeline::ForwardRenderPipeline(
    Device& device,
    SwapChainPresenter& swapChain,
    DescriptorFactory& descFactory,
    ShaderLibrary& shaderLibaray,
    FontAtlasUploadGraphBuilder& fontUploadBuilder) :
    m_device{ device },
    m_swapChain{ swapChain },
    m_descFactory{ descFactory },
    m_renderers{ device, shaderLibaray },
    m_viewTargetRes{ device, descFactory },
    m_inspectorRenderers{ device, shaderLibaray },
    // RGResourceID: static 함수이므로 생성자 초기화 리스트에서 바로 생성 가능
    m_hBackBuffer{ RenderGraph::CreateRGResourceID() },
    m_hShadow{ RenderGraph::CreateRGResourceID() },
    //Builders
    m_fontUploadBuilder{ fontUploadBuilder },
    m_clearBuilder{
        m_swapChain, m_hBackBuffer },
    m_shadowBuilder{
        m_renderers.GetShadowRenderer(),
        m_descFactory, m_shadowRes, m_hShadow },
    m_skyboxBuilder{
        m_renderers.GetSkyboxRenderer(),
        m_swapChain, m_hBackBuffer },
    m_opaqueBuilder{
        m_renderers.GetSurfRenderer(),
        m_swapChain, m_shadowRes, m_hBackBuffer, m_hShadow },
    m_debugBuilder{
        m_renderers.GetDebugSurfRenderer(),
        m_swapChain,
        m_hBackBuffer },
    m_uiBuilder{
        m_renderers.GetUIRenderer(),
        m_swapChain,
        m_hBackBuffer },
    m_inspectorBuilder{
        m_inspectorRenderers.GetInspectorImageRenderer(),
        m_hBackBuffer }
{}

bool ForwardRenderPipeline::Initialize(const Size& screenSize, const Size& shadowMapSize)
{
    ReturnIfFalse(m_shadowRes.Initialize(m_device, m_descFactory, shadowMapSize));
    ReturnIfFalse(m_renderers.Initialize());
    ReturnIfFalse(m_inspectorRenderers.Initialize(screenSize));

    return true;
}

std::vector<CompiledTask> ForwardRenderPipeline::BuildFrame(const FramePacket& framePacket)
{
    m_graph.Reset();
    m_graph.ImportResource(m_hBackBuffer, RGAccess::Present);
    m_graph.ImportResource(m_hShadow, RGAccess::DepthWrite);

    if (m_fontUploadBuilder.HasPendingUploads())
        m_fontUploadBuilder.Build(m_graph);

    m_clearBuilder.Build(m_graph);

    if (!framePacket.shadowCasters.empty())
        m_shadowBuilder.Build(m_graph, framePacket.light, framePacket.shadowCasters);

    std::vector<RenderViewInfo> renderViewInfos;
    renderViewInfos.reserve(framePacket.views.size());

    std::unordered_set<uint32_t> activeViews;

    for (size_t i = 0; i < framePacket.views.size(); ++i)
    {
        auto& view = framePacket.views[i];
        auto id = view->id;
        activeViews.insert(id);

        auto size = ToSize(
            static_cast<uint32_t>(view->viewport->width), 
            static_cast<uint32_t>(view->viewport->height));
        auto& target = m_viewTargetRes.Acquire(id, size);

        m_graph.ImportResource(target.colorID, RGAccess::RTV);
        m_graph.ImportResource(target.depthID, RGAccess::DepthWrite);

        //m_clearBuilder.Build(m_graph, target.colorID, target.depthID);
    }

    m_graph.ExportResource(m_hBackBuffer, RGAccess::Present);
    m_graph.ExportResource(m_hShadow, RGAccess::DepthWrite);        

    return m_graph.Compile();
}

//std::vector<CompiledTask> ForwardRenderPipeline::BuildFrame(const FramePacket& framePacket)
//{
//    m_graph.Reset(); // 이전 프레임의 패스/배리어 계산 결과만 비움 (컨테이너 capacity는 유지)
//
//    m_graph.ImportResource(m_hBackBuffer, RGAccess::Present);
//    m_graph.ImportResource(m_hShadow, RGAccess::DepthWrite);     
//
//    if (m_fontUploadBuilder.HasPendingUploads())
//        m_fontUploadBuilder.Build(m_graph);
//
//    m_clearBuilder.Build(m_graph);
//
//    if (!framePacket.shadowCasters.empty())
//        m_shadowBuilder.Build(m_graph, framePacket.light, framePacket.shadowCasters);
//
//    for (size_t i = 0; i < framePacket.views.size(); ++i)
//    {
//        auto& view = framePacket.views[i];
//
//        if (view->environment)
//            m_skyboxBuilder.Build(m_graph, view, i);
//
//        if (!view->surface.empty())
//            m_opaqueBuilder.Build(m_graph, framePacket.light, view, i);
//
//        if (!view->debugSurface.empty())
//            m_debugBuilder.Build(m_graph, view, i);
//
//        if (!view->ui.empty())
//            m_uiBuilder.Build(m_graph, view, i);
//    }
//
//    m_graph.ExportResource(m_hBackBuffer, RGAccess::Present);
//    m_graph.ExportResource(m_hShadow, RGAccess::DepthWrite);        
//
//    return m_graph.Compile();
//}

void ForwardRenderPipeline::Render(
    CommandList& cmd, 
    FramePacket framePacket)
{
    auto compiledTasks = BuildFrame(framePacket); // 매 프레임 그래프 재구성

    TaskContext ctx;
    ctx.resources = std::make_shared<ResourceContext>();

    m_fontUploadBuilder.ApplyResourceBindings(*ctx.resources);
    ctx.SetResource(m_hBackBuffer, m_swapChain.GetCurrentBackbuffer());
    ctx.SetResource(m_hShadow, m_shadowRes.GetResource());

    auto& bindlessAllocator = m_descFactory.GetBindlessAllocator();
    cmd.SetBindlessHeap(bindlessAllocator.GetHeap());

    ExecuteRenderPipeline(cmd, compiledTasks, ctx);
}

void ForwardRenderPipeline::Resize(const Size& size)
{
    m_inspectorRenderers.SetScreenSize(size);
}

