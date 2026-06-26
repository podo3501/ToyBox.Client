#pragma once
#include "Graph/RGTypes.h"

class RenderGraph;
class UIRenderer;

class UIGraphBuilder
{
public:
    ~UIGraphBuilder();
    UIGraphBuilder() = delete;
    UIGraphBuilder(UIRenderer& uiRenderer, RGResourceID backBufferResID);
    void Build(RenderGraph& graph);

private:
    UIRenderer& m_uiRenderer;
    RGResourceID m_backBufferResID;
};