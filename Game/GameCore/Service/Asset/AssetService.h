#pragma once

struct IResourceManager;
struct IAssetBackend;
struct IAssetLoader;

class AssetService
{
public:
	~AssetService();
	AssetService() = delete;
	static unique_ptr<AssetService> Create(IResourceManager* resManager) noexcept;
	bool RegisterLoader(string_view ext);

private:
	AssetService(unique_ptr<IAssetBackend> backend, IResourceManager* resManager) noexcept;

	unique_ptr<IAssetBackend> m_backend;
	IResourceManager* m_resManager{ nullptr };

	std::unordered_map<std::string, std::shared_ptr<IAssetLoader>> m_loaders;
};