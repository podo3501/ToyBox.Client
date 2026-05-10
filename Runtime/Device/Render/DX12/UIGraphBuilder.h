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
    UIGraphBuilder(RenderScene* scene, QuadRenderer* quadRenderer, RGHandle hBb);
    void Build(RenderGraph& graph);

private:
    RenderScene* m_scene{ nullptr };
    QuadRenderer* m_quadRenderer{ nullptr };
    RGHandle m_hBb;
};