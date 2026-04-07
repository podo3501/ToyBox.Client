#include "pch.h"
#include "AssetService.h"
#include "IAssetBackend.h"
#include "Platform/Resource/IResourceManager.h"

AssetService::~AssetService() = default;
AssetService::AssetService(unique_ptr<IAssetBackend> backend, IResourceManager* resManager) noexcept :
	m_backend{ move(backend) },
	m_resManager{ resManager }
{}

unique_ptr<AssetService> AssetService::Create(IResourceManager* resManager) noexcept
{
	if (!resManager) return nullptr;

	auto backend = CreateSTBAssetBackend();
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