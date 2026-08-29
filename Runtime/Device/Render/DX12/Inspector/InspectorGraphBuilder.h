#pragma once
#include "Graph/RGTypes.h"

class RenderGraph;
class InspectorImageRenderer;

class InspectorGraphBuilder
{
public:
    ~InspectorGraphBuilder();
    InspectorGraphBuilder() = delete;
    InspectorGraphBuilder(InspectorImageRenderer& imageRenderer, RGResourceID backBufferResID);
    void Build(RenderGraph& graph, RGResourceID resID);

private:
    InspectorImageRenderer& m_imageRenderer;
    RGResourceID m_backBufferResID;
};