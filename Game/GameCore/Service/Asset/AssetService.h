#pragma once
#include <typeindex>
#include "Platform/Resource/IResourceManager.h"
#include "GameCore/Service/Asset/IAssetLoader.h"

struct IResourceManager;
struct Asset;

struct LoaderKey
{
	std::type_index type;
	std::string ext;

	bool operator==(const LoaderKey& other) const
	{
		return type == other.type && ext == other.ext;
	}
};

struct LoaderKeyHasher
{
	size_t operator()(const LoaderKey& k) const
	{
		size_t h1 = std::hash<std::type_index>()(k.type);
		size_t h2 = std::hash<std::string>()(k.ext); 
		
		return h1 ^ (h2 << 1);
	}
};

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

		LoaderKey key{ typeid(T), string(ext) };
		m_loaders[key] = move(loader);
		return true;
	}

	template<typename T>
	shared_ptr<T> Load(const filesystem::path& path);

private:
	AssetService(IResourceManager* resManager) noexcept;

	IResourceManager* m_resManager{ nullptr };
	unordered_map<LoaderKey, unique_ptr<IAssetLoader>, LoaderKeyHasher> m_loaders;
	unordered_map<string, weak_ptr<Asset>> m_cache;
};

template<typename T>
shared_ptr<T> AssetService::Load(const filesystem::path& path)
{
	string cacheKey = path.string() + typeid(T).name();

	auto it = m_cache.find(cacheKey);
	if (it != m_cache.end())
	{
		if (auto cached = it->second.lock())
			return static_pointer_cast<T>(cached);
	}

	Core::ByteBuffer buffer;
	if (!m_resManager->Read(path, buffer))
		return nullptr;

	string ext = path.extension().string();

	LoaderKey key{ typeid(T), ext };
	auto loaderIt = m_loaders.find(key);
	if (loaderIt == m_loaders.end())
		return nullptr;

	auto asset = loaderIt->second->Load(buffer);
	if (!asset)
		return nullptr;

	m_cache[cacheKey] = asset;

	return static_pointer_cast<T>(asset);
}