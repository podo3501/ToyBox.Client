#include "pch.h"
#include "AssetService.h"
#include "AssetData.h"
#include "Platform/Resource/IResourceManager.h"

AssetService::~AssetService() = default;
AssetService::AssetService(IResourceManager* resManager) noexcept :
	m_resManager{ resManager }
{}

unique_ptr<AssetService> AssetService::Create(IResourceManager* resManager) noexcept
{
	if (!resManager) return nullptr;
	return std::unique_ptr<AssetService>(new AssetService(resManager));
}

shared_ptr<AssetData> AssetService::Load(Core::TypeID type, const Core::ResourceID& resID)
{
	CacheKey cacheKey{ resID, type };

	{
		std::lock_guard lock(m_cacheMutex);
		auto it = m_cache.find(cacheKey);
		if (it != m_cache.end())
		{
			if (auto cached = it->second.lock())
				return cached;
		}
	}

	auto ext = string(Core::GetExtension(resID.GetValue()));
	string normalized = Core::ToLowerCopy(ext);
	LoaderKey loaderKey{ type, normalized };

	IAssetLoader* loader = nullptr;
	{
		std::lock_guard lock(m_loaderMutex);
		auto loaderIt = m_loaders.find(loaderKey);
		if (loaderIt == m_loaders.end())
			return nullptr;

		loader = loaderIt->second.get();
	}

	auto asset = LoadWithSource(loader, resID);
	if (!asset) return nullptr;

	{
		std::lock_guard lock(m_cacheMutex);

		auto it = m_cache.find(cacheKey);
		if (it != m_cache.end()) // 다른 thread가 먼저 넣었을 수도 있음
		{
			if (auto existing = it->second.lock())
				return existing;
		}
		m_cache[cacheKey] = asset;
	}

	return asset;
}

shared_ptr<AssetData> AssetService::LoadWithSource(IAssetLoader* loader, const Core::ResourceID& resID)
{
	if (resID.GetType() != Core::ResourceIDType::Path) 
		return nullptr;

	const auto path = std::filesystem::path(resID.GetValue());
    if (loader->PreferStream())
    {
        auto stream = m_resManager->CreateReadStream(path);
        if (!stream) return nullptr;

        StreamInput source(std::move(stream));
        return loader->Load(source);
    }

    Core::ByteBuffer buffer;
    if (!m_resManager->Read(path, buffer)) return nullptr;

    MemoryInput source(std::move(buffer));
    return loader->Load(source);
}