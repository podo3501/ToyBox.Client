#pragma once
#include "AssetInput.h"
#include <string>

struct AssetData;
struct IAssetLoader
{
public:
	virtual ~IAssetLoader() = default;

	virtual bool PreferStream() const { return false; }
	virtual std::shared_ptr<AssetData> Load(AssetInput& source) = 0;
};

template <typename T>
std::unique_ptr<IAssetLoader> CreateLoader()
{
	static_assert(std::is_base_of_v<IAssetLoader, T>);
	return std::make_unique<T>();
}

struct IAssetMetaRegistry;

std::unique_ptr<IAssetLoader> CreateTextureMetaLoader();
std::unique_ptr<IAssetLoader> CreateImageTextureLoader(IAssetMetaRegistry* metaRegistry);
std::unique_ptr<IAssetLoader> CreateKtxTextureLoader(IAssetMetaRegistry* metaRegistry);
std::unique_ptr<IAssetLoader> CreateMeshJsonLoader();
std::unique_ptr<IAssetLoader> CreateOggStaticLoader();
std::unique_ptr<IAssetLoader> CreateWavStaticLoader();
std::unique_ptr<IAssetLoader> CreateStaticSoundTableLoader();
std::unique_ptr<IAssetLoader> CreateStreamSoundTableLoader();
std::unique_ptr<IAssetLoader> CreateOggStreamLoader();
std::unique_ptr<IAssetLoader> CreateHLSLShaderLoader();
std::unique_ptr<IAssetLoader> CreateFontLoader();

