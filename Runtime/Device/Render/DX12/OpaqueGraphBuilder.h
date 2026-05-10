#pragma once
#include "RGTypes.h"

class RenderGraph;
class RenderScene;
class MeshRenderer;

class OpaqueGraphBuilder
{
public:
    ~OpaqueGraphBuilder();
    OpaqueGraphBuilder() = delete;
    OpaqueGraphBuilder(RenderScene* scene, MeshRenderer* meshRenderer, RGHandle hBb);
    void Build(RenderGraph& graph);
    
private:
    RenderScene* m_scene{ nullptr };
    MeshRenderer* m_meshRenderer{ nullptr };
    RGHandle m_hBb;
};