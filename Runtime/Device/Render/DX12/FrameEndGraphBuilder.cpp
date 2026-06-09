#include "pch.h"
#include "FrameEndGraphBuilder.h"
#include "RenderPass.h"
#include "RenderGraph.h"

FrameEndGraphBuilder::FrameEndGraphBuilder(RGHandle hBb, RGHandle hShadow) :
    m_hBb{ hBb },
    m_hShadow{ hShadow }
{}

void FrameEndGraphBuilder::Build(RenderGraph& graph)
{
    auto& end = graph.AddPass("Present", CommandType::Direct); //barrier를 만들기 위한 pass
    end.dependsOn.push_back("UI");
    end.writes.push_back({ m_hBb, RGAccess::Present });
    end.writes.push_back({ m_hShadow, RGAccess::DepthWrite });
}