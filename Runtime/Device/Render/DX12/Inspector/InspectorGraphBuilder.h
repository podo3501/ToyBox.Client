#pragma once
#include "Graph/RGTypes.h"

class RenderGraph;
class InspectorImageRenderer;

class InspectorGraphBuilder
{
public:
    ~InspectorGraphBuilder();
    InspectorGraphBuilder() = delete;
    explicit InspectorGraphBuilder(InspectorImageRenderer& imageRenderer) noexcept;
    void Build(
        RenderGraph& graph, 
        RGResourceID backBufferResID, 
        RGResourceID resID);

private:
    InspectorImageRenderer& m_imageRenderer;
    RGResourceID m_backBufferResID;
};