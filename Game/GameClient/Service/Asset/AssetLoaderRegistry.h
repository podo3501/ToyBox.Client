#pragma once

struct IAssetLoader;
class AssetRepository;
class AssetLoaderRegistry
{
public:
    ~AssetLoaderRegistry();
    explicit AssetLoaderRegistry(AssetRepository& repository) noexcept;
    bool RegisterDefaultLoaders();

private:
    template <typename AssetType>
    bool RegisterLoader(const std::string& ext, std::unique_ptr<IAssetLoader>&& loader);

    AssetRepository& m_repository;
};