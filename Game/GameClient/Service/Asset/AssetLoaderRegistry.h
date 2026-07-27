#pragma once

struct IAssetLoader;
struct IAssetMetaRegistry;
class AssetRepository;

class AssetLoaderRegistry
{
public:
    ~AssetLoaderRegistry();
    explicit AssetLoaderRegistry(AssetRepository& repository) noexcept;
    bool RegisterDefaultLoaders(IAssetMetaRegistry* metaRegistry);

private:
    template <typename AssetType>
    bool RegisterLoader(std::string_view ext, std::unique_ptr<IAssetLoader>&& loader);

    AssetRepository& m_repository;
};