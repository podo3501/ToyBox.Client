#pragma once
#include "IAssetMetaRegistry.h"

struct IAssetAsyncLoader;

class AssetMetaRegistry : public IAssetMetaRegistry
{
public:
	virtual ~AssetMetaRegistry();
	AssetMetaRegistry() = delete;
	
	virtual std::shared_ptr<AssetData> GetMeta(const Core::ResourceID& resID) const override;

	static unique_ptr<AssetMetaRegistry> Create(IAssetAsyncLoader* asyncLoader) noexcept;
	bool Initialize();
	bool Scan(const filesystem::path& resPath);

private:
	AssetMetaRegistry(IAssetAsyncLoader* loader);
	bool RegisterMetaType(std::string_view originalExt, Core::TypeID metaTypeID);

	IAssetAsyncLoader* m_asyncLoader{ nullptr };
	std::unordered_map<std::string, Core::TypeID> m_extToMetaType;
	std::unordered_map<Core::ResourceID, std::shared_ptr<AssetData>> m_metaAssets;
};

