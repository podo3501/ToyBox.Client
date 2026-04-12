#pragma once
#include "AssetInput.h"

struct Asset;
struct IAssetLoader
{
public:
	virtual ~IAssetLoader() = default;

	virtual bool PreferStream() const { return false; }
	virtual std::shared_ptr<Asset> Load(const AssetInput& source) = 0;
};

template <typename T>
std::unique_ptr<IAssetLoader> CreateLoader()
{
	static_assert(std::is_base_of_v<IAssetLoader, T>);
	return std::make_unique<T>();
}
