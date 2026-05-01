#include "pch.h"
#include "MeshGraphBuilder.h"
#include "RenderGraph.h"
#include "RenderPass.h"
#include "TaskScheduler.h"

MeshGraphBuilder::~MeshGraphBuilder() = default;

RGResource MeshGraphBuilder::LoadMesh(std::shared_ptr<MeshAsset> asset)
{
	RenderGraph graph;
	RGResource mesh = graph.CreateResource();
	auto resources = std::make_shared<ResourceContext>();

	BuildGraph(graph, asset, mesh);

	return mesh;
}

void MeshGraphBuilder::BuildGraph(RenderGraph& graph, std::shared_ptr<MeshAsset> asset,
	RGResource texRes)
{

}