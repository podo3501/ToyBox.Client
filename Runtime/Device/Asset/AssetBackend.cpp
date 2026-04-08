#include "pch.h"
#include "AssetBackend.h"
#include "Loader/PngTextureLoader.h"
#include "GameCore/Service/Asset/IAssetLoaders.h"

AssetBackend::~AssetBackend() = default;
AssetBackend::AssetBackend() = default;

shared_ptr<IAssetLoader> AssetBackend::GetLoaderForExtension(string_view ext)
{
	if(ext == ".png") return make_shared<PngTextureLoader>();

	return nullptr;
}

///////////////////////////////////////////////////

unique_ptr<IAssetBackend> CreateAssetBackend()
{
	return make_unique<AssetBackend>();
}

