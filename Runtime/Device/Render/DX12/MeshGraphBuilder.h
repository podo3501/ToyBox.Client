#pragma once
#include "RGTypes.h"

struct MeshAsset;
class RenderGraph;

class MeshGraphBuilder
{
public:
    ~MeshGraphBuilder();
    RGResource LoadMesh(std::shared_ptr<MeshAsset> asset);

private:
    void BuildGraph(RenderGraph& graph, std::shared_ptr<MeshAsset> asset, RGResource texRes);
};
