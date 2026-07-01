#pragma once
#include "GameCore/Service/Asset/IAssetLoader.h"
#include "AssetKeys.h"
#include "Core/Utils/StringUtils.h"
#include <mutex>

struct IResourceManager;
struct AssetData;

//이 클래스는 core에 두고 core나 client가 loader를 등록해서 사용하는 형식으로 한다.
//core 클래스는 os나 dx 이런것에 의존적이면 안되기 때문이다.
class AssetService
{
public:
	~AssetService();
	AssetService() = delete;
	static unique_ptr<AssetService> Create(IResourceManager* resManager) noexcept;
	
	std::shared_ptr<AssetData> Load(Core::TypeID type, const Core::ResourceID& resID);
	template<typename T>
	std::shared_ptr<T> Load(const Core::ResourceID& resID) //Load 편의용 함수.
	{
		return std::static_pointer_cast<T>(Load(Core::GetTypeID<T>(), resID));
	}
	template<typename T>
	std::shared_ptr<T> Load(std::string_view path)
	{
		return std::static_pointer_cast<T>(
			Load(Core::GetTypeID<T>(),
				Core::ResourceID::MakePath(path)));
	}

	template<typename T>
	bool RegisterLoader(string_view ext, unique_ptr<IAssetLoader> loader)
	{
		if (!loader) return false;

		std::string normalized = Core::ToLower(ext);
		LoaderKey loaderKey{ Core::GetTypeID<T>(), normalized };

		std::lock_guard lock(m_loaderMutex);
		m_loaders[loaderKey] = move(loader);
		return true;
	}

private:
	AssetService(IResourceManager* resManager) noexcept;
	shared_ptr<AssetData> LoadWithSource(IAssetLoader* loader, const Core::ResourceID& resID);

	IResourceManager* m_resManager{ nullptr };
	unordered_map<LoaderKey, unique_ptr<IAssetLoader>, LoaderKeyHasher> m_loaders;
	mutable std::mutex m_loaderMutex;

	unordered_map<CacheKey, weak_ptr<AssetData>, CacheKeyHasher> m_cache;
	mutable std::mutex m_cacheMutex;
};

#include "AssetHelper.hpp"