#include "pch.h"
#include "AssetRepository.h"
#include "Core/Utils/StringUtils.h"
#include "Platform/Resource/IResourceManager.h"

AssetRepository::~AssetRepository() = default;
AssetRepository::AssetRepository(IResourceManager* resManager) noexcept :
	m_resManager{ resManager }
{}

bool AssetRepository::RegisterLoader(AssetLoaderDesc desc)
{
	if (!desc.loader) return false;

	std::string normalized = Core::ToLowerCopy(desc.extension);
	LoaderKey loaderKey{ desc.type, normalized };

	m_loaders[loaderKey] = move(desc.loader);
	return true;
}

shared_ptr<AssetData> AssetRepository::Load(Core::TypeID type, const Core::ResourceID& resID)
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
	auto loaderIt = m_loaders.find(loaderKey);
	Assert(loaderIt != m_loaders.end()); //파일 확장자에 맞는 로더 등록이 안돼 있다.
	if (loaderIt == m_loaders.end())
		return nullptr;

	loader = loaderIt->second.get();
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

shared_ptr<AssetData> AssetRepository::LoadWithSource(IAssetLoader* loader, const Core::ResourceID& resID)
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