#include "pch.h"
#include "ViewTargetClearGraphBuilder.h"
#include "Graph/RenderGraph.h"
#include "Command/CommandList.h"

void ViewTargetClearGraphBuilder::Build(
    RenderGraph& graph,
    RGResourceID colorID,
    RGResourceID depthID,
    size_t viewIndex)
{
    auto& clear = graph.AddGraphicsPass("ClearViewTarget_View" + std::to_string(viewIndex));
    clear.Write(colorID, RGAccess::RTV);
    clear.Write(depthID, RGAccess::DepthWrite);

    //clear.gpuExecute =
    //    [colorID, depthID]
    //    (CommandList& cmd, TaskContext& ctx)
    //    {
    //        // PMA 컨벤션: 완전 투명 = RGBA 모두 0
    //        cmd.ClearRenderTarget(ctx.GetRTV(colorID), 0.0f, 0.0f, 0.0f, 0.0f);
    //        cmd.ClearDepth(ctx.GetDSV(depthID), 1.0f);
    //    };
}