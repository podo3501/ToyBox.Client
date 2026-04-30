#include "pch.h"
#include "RenderRepository.h"
#include "MaterialRepository.h"
#include "TextureRepository.h"
#include "MeshRepository.h"
#include "../IRenderBackend.h"

RenderRepository::~RenderRepository() = default;
RenderRepository::RenderRepository(IRenderBackend* backend) :
	m_backend{ backend },
	m_texRepository{ make_unique<TextureRepository>(m_backend->GetTextureSystem()) },
	m_matRepository{ make_unique<MaterialRepository>(m_texRepository.get()) },
	m_meshRepository{ make_unique<MeshRepository>(m_backend->GetMeshSystem()) }
{}

MeshHandle RenderRepository::LoadMesh(const filesystem::path& path, function<shared_ptr<MeshAsset>(const filesystem::path&)> loader)
{
	return m_meshRepository->GetOrCreate(path, loader);
}

bool RenderRepository::ReleaseMesh(MeshHandle mh)
{
	return m_meshRepository->Release(mh);
}

TextureHandle RenderRepository::LoadTexture(const filesystem::path& path, const TextureDesc& desc,
	function<shared_ptr<TextureAsset>(const filesystem::path&)> loader)
{
	return m_texRepository->GetOrCreate(path, desc, loader);
}

bool RenderRepository::ReleaseTexture(TextureHandle th)
{
	return m_texRepository->Release(th);
}

MaterialHandle RenderRepository::CreateMaterial(TextureHandle th)
{
	return m_matRepository->Create(th);
}

void RenderRepository::Update()
{
	m_meshRepository->Update();
	m_texRepository->Update();
	m_matRepository->Update();
}

const TextureEntry* RenderRepository::Get(TextureHandle handle) const noexcept 
{ 
	return m_texRepository->Get(handle); 
}

const MeshEntry* RenderRepository::Get(MeshHandle handle) const noexcept
{
	return m_meshRepository->Get(handle);
}