#include "pch.h"
#include "PresentGraphBuilder.h"
#include "RenderPass.h"
#include "RenderGraph.h"

PresentGraphBuilder::PresentGraphBuilder(RGHandle hBb) :
    m_hBb{ hBb }
{}

void PresentGraphBuilder::Build(RenderGraph& graph)
{
    auto& present = graph.AddPass("Present", CommandType::Direct); //barrier를 만들기 위한 pass
    present.reads.push_back({ m_hBb, RGAccess::RTV });
    present.writes.push_back({ m_hBb, RGAccess::Present });
}