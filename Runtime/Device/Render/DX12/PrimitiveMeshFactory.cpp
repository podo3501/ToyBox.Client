#include "pch.h"
#include "PrimitiveMeshFactory.h"

std::shared_ptr<MeshAsset> PrimitiveMeshFactory::CreateUIQuad()
{
    auto asset = std::make_shared<MeshAsset>();
    asset->format = VertexFormat::UI;
    
    std::vector<UIVertex> vertices =
    {
        { -0.5f, -0.5f, 0, 1,1,1,1, 0,1 },
        { -0.5f,  0.5f, 0, 1,1,1,1, 0,0 },
        {  0.5f, -0.5f, 0, 1,1,1,1, 1,1 },
        {  0.5f,  0.5f, 0, 1,1,1,1, 1,0 },
    };

    std::vector<uint32_t> indices =
    {
        0,1,2,
        2,1,3
    };

    asset->SetVertices(vertices);
    asset->indices = std::move(indices);

    return asset;
}