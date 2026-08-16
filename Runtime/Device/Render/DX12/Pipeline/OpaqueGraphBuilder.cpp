#include "pch.h"
#include "OpaqueGraphBuilder.h"
#include "SwapChainPresenter.h"
#include "Graph/RenderGraph.h"
#include "Renderer/SurfaceRenderer.h"
#include "Resource/Mesh/MeshResource.h"
#include "Resource/Material/MaterialResource.h"
#include "Resource/Environment/EnvironmentResource.h"
#include "Resource/ShadowResource.h"

OpaqueGraphBuilder::~OpaqueGraphBuilder() = default;
OpaqueGraphBuilder::OpaqueGraphBuilder(
    SurfaceRenderer& surfRenderer, 
    SwapChainPresenter& swapChain,
    ShadowResource& shadowRes,
    RGResourceID backBufferResID, 
    RGResourceID shadowResID) :
    m_surfRenderer{ surfRenderer },
    m_swapChain{ swapChain },
    m_shadowRes{ shadowRes },
    m_backBufferResID { backBufferResID },
    m_shadowResID{ shadowResID }
{}

void OpaqueGraphBuilder::Build(RenderGraph& graph)
{
    auto& opaque = graph.AddGraphicsPass("Opaque");
    opaque.Read(m_shadowResID, RGAccess::SRV);
    opaque.Write(m_backBufferResID, RGAccess::RTV);
    opaque.gpuExecute =
        [
            &swapChain = m_swapChain,
            &surfRenderer = m_surfRenderer,
            &shadowRes = m_shadowRes
        ]
        (CommandList& cmd, TaskContext& ctx)
        {
            swapChain.SetRenderTarget(cmd);

            surfRenderer.PrepareFrame(
                ctx.frame.light,
                ctx.frame.camera,
                shadowRes.GetSRVIndex(),
                ctx.drawPacket.environment.get()
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