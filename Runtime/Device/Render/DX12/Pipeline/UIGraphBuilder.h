#pragma once
#include "Graph/RGTypes.h"

class RenderGraph;
class UIRenderer;

class UIGraphBuilder
{
public:
    ~UIGraphBuilder();
    UIGraphBuilder() = delete;
    UIGraphBuilder(UIRenderer* uiRenderer, RGHandle hBb);
    void Build(RenderGraph& graph);

private:
    UIRenderer* m_uiRenderer{ nullptr };
    RGHandle m_hBb;
};