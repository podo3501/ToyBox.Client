#include "pch.h"
#include "TextureRepository.h"
#include "ITextureSystem.h"
#include "ITextureResource.h"

struct PendingRequest
{
	filesystem::path path;
	TextureDesc desc;
	function<std::shared_ptr<TextureAsset>(const filesystem::path&)> loader;
	TextureHandle handle;
};

TextureRepository::~TextureRepository() = default;
TextureRepository::TextureRepository(ITextureSystem* texSystem) :
	m_texSystem{ texSystem }
{}

TextureHandle TextureRepository::GetOrCreate( const filesystem::path& path, const TextureDesc& desc,
	function<shared_ptr<TextureAsset>(const filesystem::path&)> loader)
{
	TextureKey key{ path, desc };

	auto it = m_cache.find(key);
	if (it != m_cache.end())
		return it->second;

	auto texRes = m_texSystem->CreateTextureResource();
	if (!texRes) return TextureHandle::Invalid();

	TextureEntry entry;
	entry.key = key;
	entry.texRes = move(texRes);
	entry.state = TextureState::Pending;

	auto handle = m_loadedTextures.Emplace(move(entry));
	m_cache[key] = handle;
	m_pending.push_back(PendingRequest{ path, desc, loader, handle });

	return handle;
}

bool TextureRepository::Release(TextureHandle th)
{
	auto entry = m_loadedTextures.Find(th);
	if (!entry) return false;

	m_cache.erase(entry->key);
	std::erase(m_loadingList, th);
	return m_loadedTextures.Remove(th);
}

void TextureRepository::Update()
{
	ProcessPending();
	ProcessLoading();
}

void TextureRepository::ProcessPending()
{
	for (auto& req : m_pending)
	{
		auto entry = m_loadedTextures.Find(req.handle);
		if (!entry) continue;
		if (entry->state != TextureState::Pending) continue; // 중복으로 들어온 경우 이미 Loading/Ready 라면 처리안함.

		auto asset = req.loader(req.path);
		if (!asset)
		{
			entry->state = TextureState::Failed;
			continue;
		}

		entry->state = TextureState::Loading;

		auto& texRes = entry->texRes;
		if (!m_texSystem->LoadFromAsset(texRes, asset, req.desc))
		{
			entry->state = TextureState::Failed;
			continue;
		}

		/*if (!texRes || !texRes->LoadFromAsset(asset, req.desc))
		{
			entry->state = TextureState::Failed;
			continue;
		}*/

		m_loadingList.push_back(req.handle);
	}

	m_pending.clear();
}

void TextureRepository::ProcessLoading()
{
	for (auto it = m_loadingList.begin(); it != m_loadingList.end(); )
	{
		auto entry = m_loadedTextures.Find(*it);
		if (!entry || !entry->texRes)
		{
			it = m_loadingList.erase(it);
			continue;
		}

		auto& tex = entry->texRes;
		if (tex->IsReady())
		{
			entry->state = TextureState::Ready;
			it = m_loadingList.erase(it);
		}
		else
			++it;
	}
}



