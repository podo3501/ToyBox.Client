#pragma once
#include "GameCore/Service/Asset/IAssetLoader.h"
#include "AssetKeys.h"
#include "Core/Utils/StringUtils.h"

struct IResourceManager;
struct Asset;

//이 클래스는 core에 두고 core나 client가 loader를 등록해서 사용하는 형식으로 한다.
//core 클래스는 os나 dx 이런것에 의존적이면 안되기 때문이다.
class AssetService
{
public:
	~AssetService();
	AssetService() = delete;
	static unique_ptr<AssetService> Create(IResourceManager* resManager) noexcept;

	template<typename T>
	bool RegisterLoader(string_view ext, unique_ptr<IAssetLoader> loader)
	{
		if (!loader) return false;

		std::string normalized = ToLower(ext);
		LoaderKey key{ Core::GetTypeId<T>(), normalized };
		m_loaders[key] = move(loader);
		return true;
	}

	template<typename T>
	shared_ptr<T> Load(const filesystem::path& path);

private:
	AssetService(IResourceManager* resManager) noexcept;
	shared_ptr<Asset> LoadWithSource(IAssetLoader* loader, const filesystem::path& path);

	IResourceManager* m_resManager{ nullptr };
	unordered_map<LoaderKey, unique_ptr<IAssetLoader>, LoaderKeyHasher> m_loaders;
	unordered_map<CacheKey, weak_ptr<Asset>, CacheKeyHasher> m_cache;
};

template<typename T>
shared_ptr<T> AssetService::Load(const filesystem::path& path)
{
	auto normalizedPath = path.lexically_normal();
	CacheKey cacheKey{ normalizedPath, Core::GetTypeId<T>() };

	auto it = m_cache.find(cacheKey);
	if (it != m_cache.end())
	{
		if (auto cached = it->second.lock())
			return static_pointer_cast<T>(cached);
	}

	string ext = path.extension().string();
	string normalized = ToLowerCopy(ext);

	LoaderKey key{ Core::GetTypeId<T>(), normalized };
	auto loaderIt = m_loaders.find(key);
	if (loaderIt == m_loaders.end())
		return nullptr;
	auto& loader = loaderIt->second;

	auto asset = LoadWithSource(loader.get(), path);
	if (!asset) return nullptr;

	m_cache[cacheKey] = asset;
	return static_pointer_cast<T>(asset);
}

#include "AssetHelper.hpp"