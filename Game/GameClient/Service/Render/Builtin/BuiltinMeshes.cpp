#include "pch.h"
#include "BuiltinMeshes.h"
#include "Asset/MeshAsset.h"
#include "../Desc/MeshDesc.h"
#include "../Repository/Mesh/MeshRepository.h"

static std::shared_ptr<MeshAsset> CreateUIQuadAsset()
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

	//std::vector<UIVertex> vertices =
	//{
	//	//   [Pos X, Y, Z]          [RGBA]         [U, V]
	//	{ 0.0f, 0.0f, 0.0f,    1.0f,1.0f,1.0f,1.0f,  0.0f, 0.0f }, // 0: 좌상단
	//	{ 1.0f, 0.0f, 0.0f,    1.0f,1.0f,1.0f,1.0f,  1.0f, 0.0f }, // 1: 우상단
	//	{ 1.0f, 1.0f, 0.0f,    1.0f,1.0f,1.0f,1.0f,  1.0f, 1.0f }, // 2: 우하단
	//	{ 0.0f, 1.0f, 0.0f,    1.0f,1.0f,1.0f,1.0f,  0.0f, 1.0f }, // 3: 좌하단
	//};

	//std::vector<uint32_t> indices =
	//{
	//	0,1,2,
	//	0,2,3
	//};

	asset->SetVertices(vertices);
	asset->indices = std::move(indices);

	return asset;
}

MeshHandle CreateBuiltinUIQuad(MeshRepository* repository)
{
    MeshDesc desc{ Core::ResourceID::MakeBuiltin("ui_quad") };
    return repository->GetOrCreate(desc, CreateUIQuadAsset());
}
