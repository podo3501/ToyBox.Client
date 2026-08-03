#pragma once
#include "GameClient/Asset/IAssetProvider.h"
#include "AssetLoaderDesc.h"
#include "AssetKeys.h"
#include <mutex>

struct IResourceManager;
class AssetRepository : public IAssetProvider
{
public:
	~AssetRepository();
	explicit AssetRepository(IResourceManager* resManager) noexcept;

	bool RegisterLoader(AssetLoaderDesc desc);
	std::shared_ptr<AssetData> Load(Core::TypeID type, const Core::ResourceID& resID);

private:
	shared_ptr<AssetData> LoadWithSource(IAssetLoader* loader, const Core::ResourceID& resID);

	IResourceManager* m_resManager{ nullptr };
	unordered_map<LoaderKey, unique_ptr<IAssetLoader>, LoaderKeyHasher> m_loaders;

	unordered_map<CacheKey, weak_ptr<AssetData>, CacheKeyHasher> m_cache;
	mutable std::mutex m_cacheMutex;
};
