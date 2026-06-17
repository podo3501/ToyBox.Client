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
struct ResolvedDrawData;
struct DefaultMaterialDescs;
class MaterialRepository;
class MeshRepository;
class ShaderRepository;

class RenderContext
{
public:
	~RenderContext();
	RenderContext() = delete;
	RenderContext(IRenderBackend* backend, AssetPipelineT* assetPipeline);
	bool Initialize(const DefaultMaterialDescs& defaultMatDescs);

	MeshHandle LoadMesh(const MeshDesc& desc, std::shared_ptr<MeshAsset> asset = nullptr);
	bool ReleaseMesh(MeshHandle mh);

	TextureHandle LoadTexture(const TextureDesc& desc);
	bool ReleaseTexture(TextureHandle th);

	MaterialHandle LoadMaterial(const MaterialDesc& desc);
	bool ReleaseMaterial(MaterialHandle mh);

	void DrawSurface(MeshHandle hM, MaterialHandle hMtl, const Core::Math::Matrix& world);
	void DrawDebugSurface(MeshHandle hM, MaterialHandle hMtl, const Core::Math::Matrix& world);
	void DrawUI(MaterialHandle hMtl, const Rect& dest, const Rect* source = nullptr);
	void Update();
	void ReleaseAll();

private:
	std::shared_ptr<MeshAsset> CreateUIQuad();
	std::optional<ResolvedDrawData> ResolveResources(MeshHandle hM, MaterialHandle hMtl);
	MaterialHandle GetDefaultMaterial(MaterialDomain matDomain) const;

	IRenderBackend* m_backend{ nullptr };
	unique_ptr<MeshRepository> m_meshRepository;
	unique_ptr<MaterialRepository> m_matRepository;

	//default(built in)
	MeshHandle m_uiQuad{};
	std::array<MaterialHandle, static_cast<size_t>(MaterialDomain::Count)> m_defaultMaterials;
};