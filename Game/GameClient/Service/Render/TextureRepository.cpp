#include "pch.h"
#include "TextureRepository.h"
#include "IRenderBackend.h"
#include "ITextureResource.h"

struct PendingRequest
{
	filesystem::path path;
	function<std::shared_ptr<TextureAsset>(const filesystem::path&)> loader;
	TextureHandle handle;
};

TextureRepository::~TextureRepository() = default;
TextureRepository::TextureRepository(IRenderBackend* backend) :
	m_backend{ backend }
{}

TextureHandle TextureRepository::GetOrCreate(const filesystem::path& path,
	function<shared_ptr<TextureAsset>(const filesystem::path&)> loader)
{
	auto it = m_cache.find(path);
	if (it != m_cache.end())
	{
		auto existing = it->second.lock();
		if (existing)
		{
			TextureEntry entry;
			entry.texRes = existing;
			if (existing->IsReady()) //먼저 로딩한 애가 있는데 그게 Ready가 아닐경우에는 loading이 끝나기를 기다려야 한다. gpu에서 로딩이 안 끝났기 때문에 또 들어올 수가 있다.
				entry.state = TextureState::Ready;
			else
			{
				entry.state = TextureState::Loading;
				entry.inLoadingList = true;
			}

			auto handle = m_loadedTextures.Emplace(entry);
			if (entry.state == TextureState::Loading)
				m_loadingList.push_back(handle);

			return handle;
		}
	}

	shared_ptr<ITextureResource> texRes = m_backend->CreateTextureResource();
	if (!texRes) return TextureHandle::Invalid();

	m_cache[path] = texRes;

	auto handle = m_loadedTextures.Emplace(TextureEntry{ texRes, TextureState::Pending, false });
	m_pending.push_back(PendingRequest{ path, loader, handle });
	return handle;
}

bool TextureRepository::Release(TextureHandle th)
{
	return m_loadedTextures.Remove(th);
}

void TextureRepository::Update()
{
	ProcessPending();
	ProcessLoading();
}

const TextureEntry* TextureRepository::Get(TextureHandle handle) const noexcept
{
	return m_loadedTextures.Find(handle);
}

void TextureRepository::ProcessPending()
{
	for (auto& req : m_pending)
	{
		auto entry = m_loadedTextures.Find(req.handle);
		if (!entry) continue;

		auto asset = req.loader(req.path);
		if (!asset)
		{
			entry->state = TextureState::Failed;
			continue;
		}

		entry->state = TextureState::Loading;

		auto& texRes = entry->texRes;
		if (!texRes->LoadFromAsset(move(asset)))
		{
			entry->state = TextureState::Failed;
			continue;
		}

		if (!entry->inLoadingList)
		{
			m_loadingList.push_back(req.handle);
			entry->inLoadingList = true;
		}
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
			entry->state = TextureState::Ready;
		else
		{
			++it;
			continue;
		}

		entry->inLoadingList = false;
		it = m_loadingList.erase(it);
	}
}



