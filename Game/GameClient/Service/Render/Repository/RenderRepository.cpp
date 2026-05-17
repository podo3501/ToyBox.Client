#include "pch.h"
#include "RenderRepository.h"
#include "MaterialRepository.h"
#include "TextureRepository.h"
#include "MeshRepository.h"
#include "../IRenderBackend.h"

RenderRepository::~RenderRepository() { m_backend->WaitIdle(); } //리소스를 RenderService가 들고 있기 때문에 gpu의 활동을 중지 시키고 리소스 삭제->backend 순으로 된다.
RenderRepository::RenderRepository(IRenderBackend* backend) :
	m_backend{ backend },
	m_texRepository{ make_unique<TextureRepository>(m_backend->GetTextureSystem()) },
	m_meshRepository{ make_unique<MeshRepository>(m_backend->GetMeshSystem()) },
	m_matRepository{ make_unique<MaterialRepository>(m_backend->GetMaterialSystem()) }
{}

MeshHandle RenderRepository::LoadMesh(const filesystem::path& path, function<shared_ptr<MeshAsset>(const filesystem::path&)> loader)
{
	return m_meshRepository->GetOrCreate(path, loader);
}

MeshHandle RenderRepository::LoadMesh(const std::string& runtimeKey, shared_ptr<MeshAsset> meshAsset)
{
	return m_meshRepository->GetOrCreate(runtimeKey, meshAsset);
}

bool RenderRepository::ReleaseMesh(MeshHandle mh)
{
	return m_meshRepository->Release(mh);
}

TextureHandle RenderRepository::LoadTexture(const TextureLoadDesc& desc,
	function<shared_ptr<TextureAsset>(const filesystem::path&)> loader)
{
	return m_texRepository->GetOrCreate(desc, loader);
}

bool RenderRepository::ReleaseTexture(TextureHandle th)
{
	return m_texRepository->Release(th);
}

MaterialHandle RenderRepository::LoadMaterial(const MaterialLoadDesc& desc, 
	function<shared_ptr<TextureAsset>(const filesystem::path&)> loader)
{
	return m_matRepository->GetOrCreate(desc, loader);
}

MaterialHandle RenderRepository::LoadMaterial(const std::string& runtimeKey, const MaterialDesc& desc)
{
	return m_matRepository->GetOrCreate(runtimeKey, desc);
}

void RenderRepository::Update()
{
	m_meshRepository->Update();
	m_texRepository->Update();
	m_matRepository->Update();
}

void RenderRepository::ReleaseAll()
{
	m_meshRepository->ReleaseAll();
	m_texRepository->ReleaseAll();
	m_matRepository->ReleaseAll();
}

const TextureEntry* RenderRepository::Get(TextureHandle handle) const noexcept 
{ 
	return m_texRepository->Get(handle); 
}

const MeshEntry* RenderRepository::Get(MeshHandle handle) const noexcept
{
	return m_meshRepository->Get(handle);
}

const MaterialEntry* RenderRepository::Get(MaterialHandle handle) const noexcept
{
	return m_matRepository->Get(handle);
}