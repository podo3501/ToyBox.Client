#include "pch.h"
#include "RenderService.h"
#include "IRenderBackend.h"
#include "ITextureResource.h"
#include "Platform/Resource/IResourceManager.h"
#include "Core/Foundation/Geometry2D.h"

struct DrawCommand
{
	std::shared_ptr<ITextureResource> texRes;

	Rect dest{};
	Rect source{};
	bool hasSource{ false };
};

RenderService::~RenderService() = default;
RenderService::RenderService(unique_ptr<IRenderBackend> backend) :
	m_backend{ move(backend) }
{}

unique_ptr<RenderService> RenderService::Create(unique_ptr<IRenderBackend> backend) noexcept
{
	unique_ptr<RenderService> service(new RenderService(move(backend)));
	return service;
}

TextureHandle RenderService::AcquireTexture(const filesystem::path& path, 
	function<shared_ptr<TextureAsset>(const filesystem::path&)> loader)
{
	shared_ptr<ITextureResource> texRes;

	auto it = m_cache.find(path);
	if (it != m_cache.end())
		texRes = it->second.lock();
	
	if (!texRes)
	{
		auto asset = loader(path);
		if (!asset) return TextureHandle::Invalid();

		texRes = CreateTextureResource(move(asset));
		if (!texRes) return TextureHandle::Invalid();

		m_cache.insert_or_assign(path, texRes);
	}

	return m_loadedTextures.Emplace(texRes);
}

shared_ptr<ITextureResource> RenderService::CreateTextureResource(shared_ptr<TextureAsset> asset)
{
	auto texRes = m_backend->CreateTextureResource();
	if (!texRes) return nullptr;

	if (!texRes->LoadFromAsset(move(asset))) return nullptr;
	return texRes;
}

bool RenderService::ReleaseTexture(TextureHandle th)
{
	return m_loadedTextures.Remove(th);
}

void RenderService::Draw(TextureHandle th, const Rect& dest, const Rect* source)
{
	auto texRes = m_loadedTextures.Find(th);
	if (!texRes) return;

	DrawCommand cmd;
	cmd.texRes = *texRes;
	cmd.dest = dest;

	if (source)
	{
		cmd.source = *source;
		cmd.hasSource = true;
	}

	m_drawQueue.push_back(std::move(cmd));
}

void RenderService::Flush()
{
	for (auto& cmd : m_drawQueue)
		m_backend->Draw(cmd.texRes.get(), cmd.dest, cmd.hasSource ? &cmd.source : nullptr);

	m_drawQueue.clear();
}

void RenderService::Resize(const Size& size)
{
	m_backend->Resize(size);
}

void RenderService::Update()
{
	m_backend->Update();
	Flush();
}