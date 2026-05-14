#pragma once
#include "TextureHandle.h"
#include "MaterialHandle.h"
#include "MeshHandle.h"
#include "TextureDesc.h"

struct TextureEntry;
struct MeshEntry;
struct MaterialEntry;
struct TextureAsset;
struct IRenderBackend;
struct MeshAsset;
class TextureRepository;
class MaterialRepository;
class MeshRepository;

class RenderRepository
{
public:
	~RenderRepository();
	RenderRepository() = delete;
	explicit RenderRepository(IRenderBackend* backend);

	MeshHandle LoadMesh(const filesystem::path& path, function<shared_ptr<MeshAsset>(const filesystem::path&)> loader);
	bool ReleaseMesh(MeshHandle mh);

	TextureHandle LoadTexture(const filesystem::path& path, const TextureDesc& desc,
		function<shared_ptr<TextureAsset>(const filesystem::path&)> loader);
	bool ReleaseTexture(TextureHandle th);

	MaterialHandle LoadMaterial(const filesystem::path& path, const TextureDesc& desc,
		function<shared_ptr<TextureAsset>(const filesystem::path&)> loader);

	void Update();
	void ReleaseAll();
	const TextureEntry* Get(TextureHandle handle) const noexcept;
	const MeshEntry* Get(MeshHandle handle) const noexcept;
	const MaterialEntry* Get(MaterialHandle handle) const noexcept;

private:
	IRenderBackend* m_backend{ nullptr };
	unique_ptr<MeshRepository> m_meshRepository;
	unique_ptr<TextureRepository> m_texRepository;
	unique_ptr<MaterialRepository> m_matRepository;
};