#include "pch.h"
#include "OpaqueGraphBuilder.h"
#include "SwapChainPresenter.h"
#include "RenderPass.h"
#include "RenderGraph.h"
#include "RenderScene.h"
#include "Renderer/SurfaceRenderer.h"
#include "MeshResource.h"
#include "MaterialResource/PhongMaterialResource.h"
#include "MaterialResource/PbrMaterialResource.h"

OpaqueGraphBuilder::~OpaqueGraphBuilder() = default;
OpaqueGraphBuilder::OpaqueGraphBuilder(
    SurfaceRenderer* surfRenderer, 
    SwapChainPresenter* swapChain,
    RenderScene* scene, 
    RGHandle hBb, RGHandle hShadow) :
    m_surfRenderer{ surfRenderer },
    m_swapChain{ swapChain },
    m_scene{ scene }, 
    m_hBb { hBb },
    m_hShadow{ hShadow }
{}

void OpaqueGraphBuilder::Build(RenderGraph& graph)
{
    auto& opaque = graph.AddPass("Opaque", CommandType::Direct);
    opaque.dependsOn.push_back("Shadow");
    opaque.writes.push_back({ m_hShadow, RGAccess::SRV });
    opaque.writes.push_back({ m_hBb, RGAccess::RTV });
    opaque.gpuExecute = [this](CommandList& cmd, TaskContext& ctx) {
        m_swapChain->SetRenderTarget(cmd);
        m_swapChain->Clear(cmd, 0.13f, 0.13f, 0.16f, 1.0f);

        m_surfRenderer->BindCommonState(cmd);
        m_surfRenderer->PrepareFrame(
            ctx.frame.light, 
            ctx.frame.camera

        );

        for (auto& item : m_scene->GetSurfaceDraws())
        {
            auto mesh = static_cast<MeshResource*>(item.mesh.get());
            auto material = static_cast<MaterialResource*>(item.material.get());

            m_surfRenderer->BindPipeline(cmd, material->GetPipelineState());
            m_surfRenderer->Draw(cmd, *mesh, *material, item.world);
        }
        };
}