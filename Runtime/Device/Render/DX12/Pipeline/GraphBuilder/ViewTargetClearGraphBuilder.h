#pragma once
#include "Graph/RGTypes.h"

class RenderGraph;

class ViewTargetClearGraphBuilder
{
public:
    void Build(
        RenderGraph& graph, 
        RGResourceID colorID, 
        RGResourceID depthID, 
        size_t viewIndex);
};