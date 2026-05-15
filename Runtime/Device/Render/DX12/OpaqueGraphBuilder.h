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
    OpaqueGraphBuilder(MeshRenderer* meshRenderer, RenderScene* scene, RGHandle hBb);
    void Build(RenderGraph& graph);
    
private:
    MeshRenderer* m_meshRenderer{ nullptr };
    RenderScene* m_scene{ nullptr };
    RGHandle m_hBb;
};