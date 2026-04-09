#include "pch.h"
#include "RenderService.h"
#include "IRenderBackend.h"
#include "Platform/Resource/IResourceManager.h"

RenderService::~RenderService() = default;
RenderService::RenderService(unique_ptr<IRenderBackend> backend) :
	m_backend{ move(backend) }
{}

unique_ptr<RenderService> RenderService::Create(unique_ptr<IRenderBackend> backend) noexcept
{
	unique_ptr<RenderService> service(new RenderService(move(backend)));
	return service;
}

int RenderService::AcquireTexture(const filesystem::path& path, 
	function<shared_ptr<TextureAsset>(const filesystem::path&)> loader)
{
	auto it = m_cache.find(path);
	if (it != m_cache.end())
		return it->second;

	auto asset = loader(path);
	if (!asset) return -1;

	int tex = m_backend->UploadTexture(*asset);
	m_cache[path] = tex;

	return tex;
}

void RenderService::Draw(int index, const Rect& dest, const Rect* source)
{
	m_backend->Draw(index, dest, source);
}

void RenderService::Resize(const Size& size)
{
	m_backend->Resize(size);
}

void RenderService::Update()
{
	m_backend->Update();
}