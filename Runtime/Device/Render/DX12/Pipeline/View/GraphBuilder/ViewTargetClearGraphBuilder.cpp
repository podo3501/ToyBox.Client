#include "pch.h"
#include "ViewTargetClearGraphBuilder.h"
#include "Graph/RenderGraph.h"
#include "Command/CommandList.h"
#include "Command/CommandListHelpers.h"
#include "Factory/DescriptorFactory.h"
#include "Resource/Internal/ViewTargetResource.h"

ViewTargetClearGraphBuilder::~ViewTargetClearGraphBuilder() = default;
ViewTargetClearGraphBuilder::ViewTargetClearGraphBuilder(DescriptorFactory& descFactory) noexcept :
    m_descFactory{ descFactory }
{}

void ViewTargetClearGraphBuilder::Build(RenderGraph& graph, const ViewTargetResource& target)
{
    auto& clear = graph.AddGraphicsPass("ClearViewTarget");
    clear.Write(target.GetColorID(), RGAccess::RTV);
    clear.Write(target.GetDepthID(), RGAccess::DepthWrite);

    clear.execute =
        [
            &descFactory = m_descFactory,
            colorRTVIndex = target.GetColorRTVIndex(),
            depthDSVIndex = target.GetDepthDSVIndex()
        ]
        (CommandList& cmd, TaskContext& ctx)
        {
            auto rtv = descFactory.GetRTVHandle(colorRTVIndex);
            auto dsv = descFactory.GetDSVHandle(depthDSVIndex);

            float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f }; // PMA 컨벤션: 완전 투명 = RGBA 모두 0
            CommandUtils::ClearRTV(cmd, rtv, clearColor);
            CommandUtils::ClearDSV(cmd, dsv, 1.0f);
        };
}