#include "pch.h"
#include "TextureRepository.h"
#include "ITextureSystem.h"
#include "ITextureResource.h"

struct CpuPendingTextureRequest
{
	std::filesystem::path path;
	TextureDesc desc;
	function<std::shared_ptr<TextureAsset>(const filesystem::path&)> loader;
	TextureHandle handle;
};

struct GpuPendingTextureRequest
{
	TextureHandle handle;
	std::shared_ptr<TextureAsset> asset;
	TextureDesc desc;
};

TextureRepository::~TextureRepository() { ReleaseAll(); }
TextureRepository::TextureRepository(ITextureSystem* texSystem) :
	m_texSystem{ texSystem }
{}

TextureHandle TextureRepository::GetOrCreate( 
	std::filesystem::path path,
	const TextureDesc& desc, 
	function<shared_ptr<TextureAsset>(const filesystem::path&)> loader)
{
	auto keyPath = std::filesystem::weakly_canonical(path);
	TextureKey key{ ResourceKey{ keyPath.string(), ResourceKey::Type::File}, desc };

	auto it = m_cache.find(key);
	if (it != m_cache.end())
		return it->second;

	auto texRes = m_texSystem->CreateTextureResource();
	if (!texRes) return TextureHandle::Invalid();

	TextureEntry entry;
	entry.key = key;
	entry.texRes = move(texRes);
	entry.state = LoadState::Pending;

	auto handle = m_loadedTextures.Emplace(move(entry));
	m_cache[key] = handle;
	m_cpuPending.push_back(CpuPendingTextureRequest{ path, desc, loader, handle });

	return handle;
}

TextureHandle TextureRepository::GetOrCreate(
	const std::string& runtimeKey, 
	std::shared_ptr<TextureAsset> asset, 
	const TextureDesc& desc)
{
	TextureKey key{ ResourceKey{ runtimeKey, ResourceKey::Type::Runtime }, desc };

	auto it = m_cache.find(key);
	if (it != m_cache.end())
		return it->second;

	auto texRes = m_texSystem->CreateTextureResource();
	if (!texRes) return TextureHandle::Invalid();

	TextureEntry entry;
	entry.key = key;
	entry.texRes = move(texRes);
	entry.state = LoadState::Pending;

	auto handle = m_loadedTextures.Emplace(move(entry));
	m_cache[key] = handle;
	m_gpuPending.push_back(GpuPendingTextureRequest{ handle, asset, desc });

	return handle;
}

void TextureRepository::Update()
{
	ProcessCpuPending();
	ProcessGpuPending();
	ProcessLoading();
}

void TextureRepository::ProcessCpuPending()
{
	for (auto& req : m_cpuPending)
	{
		auto entry = m_loadedTextures.Find(req.handle);
		if (!entry) continue;
		if (entry->state != LoadState::Pending) continue; // 중복으로 들어온 경우 이미 Loading/Ready 라면 처리안함.

		entry->state = LoadState::CpuLoading;
		auto asset = req.loader(req.path);
		if (!asset)
		{
			entry->state = LoadState::Failed;
			continue;
		}

		m_gpuPending.push_back(GpuPendingTextureRequest{ req.handle, asset, req.desc });
	}

	m_cpuPending.clear();
}

void TextureRepository::ProcessGpuPending()
{
	for(auto& work : m_gpuPending)
	{
		auto entry = m_loadedTextures.Find(work.handle);
		if (!entry || !entry->texRes) continue;

		if (!m_texSystem->LoadFromAsset(entry->texRes, work.asset, work.desc))
		{
			entry->state = LoadState::Failed;
			continue;
		}
		entry->state = LoadState::GpuLoading;

		m_loadingList.push_back(work.handle);
	}

	m_gpuPending.clear();
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
			entry->state = LoadState::Ready;
			it = m_loadingList.erase(it);
		}
		else
			++it;
	}
}

bool TextureRepository::Release(TextureHandle h)
{
	auto entry = m_loadedTextures.Find(h);
	if (!entry) return false;

	m_cache.erase(entry->key);
	std::erase(m_loadingList, h);
	return m_loadedTextures.Remove(h);
}

void TextureRepository::ReleaseAll()
{
	m_cpuPending.clear();
	m_gpuPending.clear();
	m_loadingList.clear();

	m_cache.clear();
	m_loadedTextures.Clear();
}