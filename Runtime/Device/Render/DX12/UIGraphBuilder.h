#pragma once
#include "RGTypes.h"

class RenderGraph;
class RenderScene;
class QuadRenderer;

class UIGraphBuilder
{
public:
    ~UIGraphBuilder();
    UIGraphBuilder() = delete;
    UIGraphBuilder(QuadRenderer* quadRenderer, RenderScene* scene, RGHandle hBb);
    void Build(RenderGraph& graph);

private:
    QuadRenderer* m_quadRenderer{ nullptr };
    RenderScene* m_scene{ nullptr };
    RGHandle m_hBb;
};