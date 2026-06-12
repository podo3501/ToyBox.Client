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

