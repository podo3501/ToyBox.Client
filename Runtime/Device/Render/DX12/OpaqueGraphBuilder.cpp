#include "pch.h"
#include "OpaqueGraphBuilder.h"
#include "RenderPass.h"
#include "RenderGraph.h"
#include "RenderScene.h"
#include "MeshRenderer.h"
#include "MeshResource.h"

OpaqueGraphBuilder::~OpaqueGraphBuilder() = default;
OpaqueGraphBuilder::OpaqueGraphBuilder(RenderScene* scene, MeshRenderer* meshRenderer, RGHandle hBb) :
    m_scene{ scene }, 
    m_meshRenderer{ meshRenderer }, 
    m_hBb { hBb }
{}

void OpaqueGraphBuilder::Build(RenderGraph& graph)
{
    auto& opaque = graph.AddPass("Opaque", CommandType::Direct);
    opaque.dependsOn.push_back("Prepare");
    opaque.writes.push_back({ m_hBb, RGAccess::RTV });
    opaque.gpuExecute = [this](CommandList& cmd, TaskContext& ctx) {
        m_meshRenderer->BindDescriptorHeap(cmd);
        m_meshRenderer->BindPipeline(cmd);

        for (auto& item : m_scene->GetOpaqueDraws())
        {
            DescriptorAllocation dummy{};
            auto mesh = static_cast<MeshResource*>(item.mesh.get());
            m_meshRenderer->Draw(cmd, *mesh, item.world, dummy);
        }
        };
}