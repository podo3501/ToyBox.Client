#pragma once
#include "IAssetMetaRegistry.h"

struct IAssetAsyncLoader;
class AssetMetaRegistry : public IAssetMetaRegistry
{
public:
	virtual ~AssetMetaRegistry();
	AssetMetaRegistry() = delete;

	static unique_ptr<AssetMetaRegistry> Create(IAssetAsyncLoader* asyncLoader) noexcept;
	bool Scan(const filesystem::path& resPath);

private:
	AssetMetaRegistry(IAssetAsyncLoader* loader);

	IAssetAsyncLoader* m_asyncLoader{ nullptr };
};

