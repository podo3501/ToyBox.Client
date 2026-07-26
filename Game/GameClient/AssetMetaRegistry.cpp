#include "pch.h"
#include "AssetMetaRegistry.h"
#include "Asset/TextureMetaAsset.h"
#include "Service/AssetAsyncHelper.h"

AssetMetaRegistry::~AssetMetaRegistry() = default;
AssetMetaRegistry::AssetMetaRegistry(IAssetAsyncLoader* asyncLoader) :
	m_asyncLoader{ asyncLoader }
{}

bool AssetMetaRegistry::Scan(const filesystem::path& resPath)
{
    std::vector<std::filesystem::path> metaPaths;
    for (auto& entry : std::filesystem::recursive_directory_iterator(resPath))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".meta")
            metaPaths.push_back(entry.path());
    }
    if (metaPaths.empty())
        return true;

    std::vector<AssetRequest> requests;
    requests.reserve(metaPaths.size());
    for (auto& metaPath : metaPaths)
        requests.push_back(Asset::MakeRequest<TextureMetaAsset>(metaPath.string()));

    auto requestIDs = Asset::PushRequests(m_asyncLoader, requests);
    auto results = Asset::WaitAll(m_asyncLoader, requestIDs);

	return true;
}

unique_ptr<AssetMetaRegistry> AssetMetaRegistry::Create(IAssetAsyncLoader* loader) noexcept
{
	unique_ptr<AssetMetaRegistry> instance(new AssetMetaRegistry(loader));
	return instance;
}