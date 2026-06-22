#include "pch.h"
#include "OpaqueGraphBuilder.h"
#include "SwapChainPresenter.h"
#include "Graph/RenderPass.h"
#include "Graph/RenderGraph.h"
#include "Renderer/SurfaceRenderer.h"
#include "Resource/Mesh/MeshResource.h"
#include "Resource/Material/PhongMaterialResource.h"
#include "Resource/Material/PbrMaterialResource.h"
#include "Resource/ShadowResource.h"

OpaqueGraphBuilder::~OpaqueGraphBuilder() = default;
OpaqueGraphBuilder::OpaqueGraphBuilder(
    SurfaceRenderer& surfRenderer, 
    SwapChainPresenter& swapChain,
    ShadowResource& shadowRes,
    RGHandle hBb, RGHandle hShadow) :
    m_surfRenderer{ surfRenderer },
    m_swapChain{ swapChain },
    m_shadowRes{ shadowRes },
    m_hBb { hBb },
    m_hShadow{ hShadow }
{}

void OpaqueGraphBuilder::Build(RenderGraph& graph)
{
    auto& opaque = graph.AddPass("Opaque", CommandType::Direct);
    opaque.dependsOn.push_back("Shadow");
    opaque.writes.push_back({ m_hShadow, RGAccess::SRV });
    opaque.writes.push_back({ m_hBb, RGAccess::RTV });
    opaque.gpuExecute =
        [
            &swapChain = m_swapChain,
            &surfRenderer = m_surfRenderer,
            &shadowRes = m_shadowRes
        ]
        (CommandList& cmd, TaskContext& ctx)
        {
            swapChain.SetRenderTarget(cmd);
            swapChain.Clear(cmd, 0.13f, 0.13f, 0.16f, 1.0f);

            surfRenderer.PrepareFrame(
                ctx.frame.light,
                ctx.frame.camera,
                shadowRes.GetSRVIndex()
            );
            surfRenderer.BeginFrame(cmd);

            for (auto& item : ctx.drawPacket.surface)
            {
                auto mesh = static_cast<MeshResource*>(item.mesh.get());
                auto material = static_cast<MaterialResource*>(item.material.get());

                surfRenderer.BindPipeline(cmd, material->GetPipelineState());
                surfRenderer.Draw(cmd, *mesh, *material, item.world);
            }
        };
}