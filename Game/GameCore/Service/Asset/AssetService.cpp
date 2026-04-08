#include "pch.h"
#include "AssetService.h"
#include "AssetTypes.h"
#include "IAssetBackend.h"
#include "IAssetLoaders.h"
#include "Platform/Resource/IResourceManager.h"

AssetService::~AssetService() = default;
AssetService::AssetService(unique_ptr<IAssetBackend> backend, IResourceManager* resManager) noexcept :
	m_backend{ move(backend) },
	m_resManager{ resManager }
{}

unique_ptr<AssetService> AssetService::Create(IResourceManager* resManager) noexcept
{
	if (!resManager) return nullptr;

	auto backend = CreateAssetBackend();
	unique_ptr<AssetService> service(new AssetService(move(backend), resManager));
	return service;
}

bool AssetService::RegisterLoader(string_view ext)
{
	auto loader = m_backend->GetLoaderForExtension(ext);
	if (!loader) return false;

	m_loaders[string(ext)] = loader;
	return true;
}

shared_ptr<Asset> AssetService::LoadInternal(const filesystem::path& path)
{
    string key = path.string();

    auto it = m_cache.find(key);
    if (it != m_cache.end())
    {
        if (auto cached = it->second.lock())
            return cached;
    }

    Core::ByteBuffer buffer;
    if (!m_resManager->Read(path, buffer))
        return nullptr;

    string ext = path.extension().string();
    auto loaderIt = m_loaders.find(ext);
    if (loaderIt == m_loaders.end()) return nullptr;

    auto asset = loaderIt->second->Load(buffer);
    if (!asset) return nullptr;

    m_cache[key] = asset;

    return asset;
}