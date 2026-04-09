#include "pch.h"
#include "AssetService.h"
#include "Asset.h"

AssetService::~AssetService() = default;
AssetService::AssetService(IResourceManager* resManager) noexcept :
	m_resManager{ resManager }
{}

unique_ptr<AssetService> AssetService::Create(IResourceManager* resManager) noexcept
{
	if (!resManager) return nullptr;

	unique_ptr<AssetService> service(new AssetService(resManager));
	return service;
}