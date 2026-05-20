#include "pch.h"
#include "RenderContext.h"
#include "Repository/Material/MaterialRepository.h"
#include "Repository/Texture/TextureRepository.h"
#include "Repository/Mesh/MeshRepository.h"
#include "Repository/Shader/ShaderRepository.h"
#include "Desc/MeshMaterialDesc.h"
#include "Desc/UIMaterialDesc.h"
#include "IRenderBackend.h"

RenderContext::~RenderContext() { m_backend->WaitIdle(); } //리소스를 RenderService가 들고 있기 때문에 gpu의 활동을 중지 시키고 리소스 삭제->backend 순으로 된다.
RenderContext::RenderContext(IRenderBackend* backend) :
	m_backend{ backend },
	m_texRepository{ make_unique<TextureRepository>(m_backend->GetTextureSystem()) },
	m_meshRepository{ make_unique<MeshRepository>(m_backend->GetMeshSystem()) },
	m_matRepository{ make_unique<MaterialRepository>(m_backend->GetMaterialSystem()) },
	m_shaderRepository{ make_unique<ShaderRepository>(m_backend->GetShaderSystem()) }
{}

MeshHandle RenderContext::LoadMesh(const filesystem::path& path, function<shared_ptr<MeshAsset>(const filesystem::path&)> loader)
{
	return m_meshRepository->GetOrCreate(path, loader);
}

MeshHandle RenderContext::LoadMesh(const std::string& runtimeKey, shared_ptr<MeshAsset> meshAsset)
{
	return m_meshRepository->GetOrCreate(runtimeKey, meshAsset);
}

bool RenderContext::ReleaseMesh(MeshHandle mh)
{
	return m_meshRepository->Release(mh);
}

TextureHandle RenderContext::LoadTexture(
	std::filesystem::path path,
	const TextureDesc& desc,
	function<shared_ptr<TextureAsset>(const filesystem::path&)> loader)
{
	return m_texRepository->GetOrCreate(path, desc, loader);
}

bool RenderContext::ReleaseTexture(TextureHandle th)
{
	return m_texRepository->Release(th);
}

MaterialHandle RenderContext::LoadMaterial(
	const std::filesystem::path& path,
	unique_ptr<MaterialDesc> desc,
	function<shared_ptr<TextureAsset>(const filesystem::path&)> loader)
{
	return m_matRepository->GetOrCreate(path, std::move(desc), loader);
}

MaterialHandle RenderContext::LoadMaterial(
	const std::string& runtimeKey, 
	shared_ptr<TextureAsset> texAsset,
	unique_ptr<MaterialDesc> desc)
{
	return m_matRepository->GetOrCreate(runtimeKey, texAsset, std::move(desc));
}

MaterialHandle RenderContext::LoadMaterial(
	const std::string& runtimeKey,
	MaterialType matType,
	shared_ptr<TextureAsset> texAsset)
{
	std::unique_ptr<MaterialDesc> desc;

	switch (matType)
	{
	case MaterialType::Mesh: desc = std::make_unique<MeshMaterialDesc>(); break;
	case MaterialType::UI: desc = std::make_unique<UIMaterialDesc>(); break;
	}

	return LoadMaterial(runtimeKey, texAsset, std::move(desc));
}

bool RenderContext::RegisterShader(
	const std::filesystem::path& path, 
	ShaderID shaderID, 
	std::function<shared_ptr<ShaderAsset>(const filesystem::path&)> loader)
{
	return m_shaderRepository->RegisterShader(path, shaderID, loader);
}

void RenderContext::Update()
{
	m_meshRepository->Update();
	m_texRepository->Update();
	m_matRepository->Update();
}

void RenderContext::ReleaseAll()
{
	m_meshRepository->ReleaseAll();
	m_texRepository->ReleaseAll();
	m_matRepository->ReleaseAll();
}

const TextureEntry* RenderContext::Get(TextureHandle handle) const noexcept
{ 
	return m_texRepository->Get(handle); 
}

const MeshEntry* RenderContext::Get(MeshHandle handle) const noexcept
{
	return m_meshRepository->Get(handle);
}

const MaterialEntry* RenderContext::Get(MaterialHandle handle) const noexcept
{
	return m_matRepository->Get(handle);
}