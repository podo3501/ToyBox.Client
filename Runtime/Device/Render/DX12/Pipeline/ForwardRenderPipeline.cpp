#include "pch.h"
#include "ForwardRenderPipeline.h"
#include "Graph/RenderGraph.h"
#include "Renderer/Renderers.h"
#include "Resource/ShadowResource.h"
#include "Scene/RenderScene.h"
#include "Descriptor/DescriptorFactory.h"
#include "ShadowGraphBuilder.h"
#include "OpaqueGraphBuilder.h"
#include "DebugSurfaceGraphBuilder.h"
#include "UIGraphBuilder.h"
#include "FrameEndGraphBuilder.h"

ForwardRenderPipeline::~ForwardRenderPipeline() = default;
ForwardRenderPipeline::ForwardRenderPipeline(Renderers* renderers, DescriptorFactory* descFactory) :
    m_renderers{ renderers },
    m_descFactory{ descFactory }
{}
//
//void ForwardRenderPipeline::BuildGraph()
//{
//    m_hBackBuffer = m_graph.CreateRGHandle();
//    m_hShadow = m_graph.CreateRGHandle();
//
//    m_graph.ImportResource(
//        m_hBackBuffer,
//        RGAccess::Present);
//
//    m_graph.ImportResource(
//        m_hShadow,
//        RGAccess::DepthWrite);
//
//    ShadowGraphBuilder shadow(
//        m_renderers->GetShadowRenderer(),
//        m_descFactory,
//        m_shadowRes,
//        nullptr,
//        m_hShadow);
//
//    OpaqueGraphBuilder opaque(
//        m_renderers->GetSurfRenderer(),
//        m_swapChain,
//        m_shadowRes,
//        nullptr,
//        m_hBackBuffer,
//        m_hShadow);
//
//    DebugSurfaceGraphBuilder debug(
//        m_renderers->GetDebugSurfRenderer(),
//        nullptr,
//        m_hBackBuffer);
//
//    UIGraphBuilder ui(
//        m_renderers->GetUIRenderer(),
//        nullptr,
//        m_hBackBuffer);
//
//    FrameEndGraphBuilder end(
//        m_hBackBuffer,
//        m_hShadow);
//
//    shadow.Build(m_graph);
//    opaque.Build(m_graph);
//    debug.Build(m_graph);
//    ui.Build(m_graph);
//    end.Build(m_graph);
//
//    m_compiledTasks = m_graph.Compile();
//}

void ForwardRenderPipeline::BuildFrame(RenderGraph& graph, RenderScene* scene, SwapChainPresenter* swapChain, ShadowResource* shadowRes)
{
    //graph.ImportResource(hBb, RGAccess::Present); //backbuffer가 present에서 시작한다고 알려준다.
    //graph.ImportResource(hShadow, RGAccess::DepthWrite);

    //ShadowGraphBuilder shadow(m_renderers->GetShadowRenderer(),
    //    m_descFactory.get(), m_shadowRes.get(), m_scene.get(), hShadow);
    //OpaqueGraphBuilder opaque(m_renderers->GetSurfRenderer(),
    //    m_swapChain.get(), m_shadowRes.get(), m_scene.get(), hBb, hShadow);
    //DebugSurfaceGraphBuilder debugSurface(m_renderers->GetDebugSurfRenderer(), m_scene.get(), hBb);
    //UIGraphBuilder ui(m_renderers->GetUIRenderer(), m_scene.get(), hBb);
    //FrameEndGraphBuilder end(hBb, hShadow);

    //shadow.Build(graph);
    //opaque.Build(graph);
    //debugSurface.Build(graph);
    //ui.Build(graph);
    //end.Build(graph);
}

