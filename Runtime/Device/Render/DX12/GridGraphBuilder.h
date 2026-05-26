#pragma once
#include "RGTypes.h"

class RenderGraph;
class RenderScene;
class GridRenderer;

class GridGraphBuilder
{
public:
    ~GridGraphBuilder();
    GridGraphBuilder() = delete;
    GridGraphBuilder(GridRenderer* gridRenderer, RenderScene* scene, RGHandle hBb);
    void Build(RenderGraph& graph);

private:
    GridRenderer* m_gridRenderer{ nullptr };
    RenderScene* m_scene{ nullptr };
    RGHandle m_hBb;
};