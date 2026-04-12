#include "pch.h"
#include "AssetService.h"
#include "Asset.h"
#include "Platform/Resource/IResourceManager.h"

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

shared_ptr<Asset> AssetService::LoadWithSource(IAssetLoader* loader, const filesystem::path& path)
{
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