#include "pch.h"
#include "BuiltinMeshes.h"
#include "Asset/MeshAsset.h"
#include "../Definition/Mesh/MeshDesc.h"
#include "../Repository/Mesh/MeshRepository.h"

static std::shared_ptr<MeshAsset> CreateUIQuadAsset()
{
	auto asset = std::make_shared<MeshAsset>();
	asset->format = VertexFormat::UI;

	std::vector<UIVertex> vertices =
	{
		// mode = 0 (일반 UI/비트맵 모드), textureIndex = 0 (기본값)
		{ { 0.0f, 0.0f, 0.0f }, Core::Color::White, { 0.0f, 0.0f }, UIRenderMode::UI, 0 }, // 0, 좌상단
		{ { 1.0f, 0.0f, 0.0f }, Core::Color::White, { 1.0f, 0.0f }, UIRenderMode::UI, 0 }, // 1, 우상단
		{ { 1.0f, 1.0f, 0.0f }, Core::Color::White, { 1.0f, 1.0f }, UIRenderMode::UI, 0 }, // 2, 우하단
		{ { 0.0f, 1.0f, 0.0f }, Core::Color::White, { 0.0f, 1.0f }, UIRenderMode::UI, 0 }, // 3, 좌하단
	};

	std::vector<uint32_t> indices =
	{
		0,1,2,
		0,2,3
	};

	asset->SetVertices(vertices);
	asset->indices = std::move(indices);

	return asset;
}

MeshHandle CreateBuiltinUIQuad(MeshRepository* repository)
{
    MeshDesc desc{ Core::ResourceID::MakeBuiltin("ui_quad") };
    return repository->GetOrCreate(desc, CreateUIQuadAsset());
}
