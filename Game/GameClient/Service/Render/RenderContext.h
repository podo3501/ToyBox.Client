#pragma once
#include "Handle/TextureHandle.h"
#include "Handle/MaterialHandle.h"
#include "Handle/MeshHandle.h"
#include "Desc/MaterialDesc.h"
#include "Desc/TextureDesc.h"
#include "Core/Math/Matrix.h"
#include "Core/Foundation/Geometry2D.h"
#include "../AssetAsync/AssetAsyncTypes.h"

struct TextureEntry;
struct MeshEntry;
struct MaterialEntry;
struct TextureAsset;
struct IRenderBackend;
struct MeshAsset;
struct ShaderAsset;
struct MeshDesc;
struct MeshMaterialDesc;
class TextureRepository;
class MaterialRepository;
class MeshRepository;
class ShaderRepository;

class RenderContext
{
public:
	~RenderContext();
	RenderContext() = delete;
	RenderContext(IRenderBackend* backend, AssetPipelineT* assetPipeline);
	bool Initialize();

	MeshHandle LoadMesh(const MeshDesc& desc, std::shared_ptr<MeshAsset> asset = nullptr);
	bool ReleaseMesh(MeshHandle mh);

	TextureHandle LoadTexture(const TextureDesc& desc);
	bool ReleaseTexture(TextureHandle th);

	MaterialHandle LoadMaterial(const MaterialDesc& desc);



	//MaterialHandle LoadMaterial(
	//	unique_ptr<MaterialDesc> desc,
	//	function<shared_ptr<TextureAsset>(const filesystem::path&)> loader);

	//MaterialHandle LoadMaterial(
	//	const std::filesystem::path& path,
	//	unique_ptr<MaterialDesc> desc,
	//	function<shared_ptr<TextureAsset>(const filesystem::path&)> loader);

	//MaterialHandle LoadMaterial(
	//	const std::string& runtimeKey, 
	//	shared_ptr<TextureAsset> albedoAsset,
	//	unique_ptr<MaterialDesc> desc);

	//MaterialHandle LoadMaterial(
	//	const std::string& runtimeKey,
	//	MaterialType matType,
	//	shared_ptr<TextureAsset> albedoAsset = nullptr);

	bool ReleaseMaterial(MaterialHandle mh);

	void DrawMesh(
		MeshHandle hM,
		MaterialHandle hMtl,
		const Core::Math::Matrix& world);

	void DrawUI(
		MaterialHandle mh,
		const Rect& dest,
		const Rect* source = nullptr);

	//void DrawGrid(
	//	MeshHandle hM,
	//	MaterialHandle hMtl,
	//	const cm::Matrix& world);

	void Update();
	void ReleaseAll();

private:
	std::shared_ptr<MeshAsset> CreateUIQuad();

	IRenderBackend* m_backend{ nullptr };
	unique_ptr<MeshRepository> m_meshRepository;
	unique_ptr<TextureRepository> m_texRepository;
	unique_ptr<MaterialRepository> m_matRepository;

	MeshHandle m_uiQuad;
};