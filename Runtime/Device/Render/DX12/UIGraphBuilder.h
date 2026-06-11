#pragma once
#include "Graph/RGTypes.h"

class RenderGraph;
class RenderScene;
class UIRenderer;

class UIGraphBuilder
{
public:
    ~UIGraphBuilder();
    UIGraphBuilder() = delete;
    UIGraphBuilder(UIRenderer* uiRenderer, RenderScene* scene, RGHandle hBb);
    void Build(RenderGraph& graph);

private:
    UIRenderer* m_uiRenderer{ nullptr };
    RenderScene* m_scene{ nullptr };
    RGHandle m_hBb;
};