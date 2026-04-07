#include "pch.h"
#include "STBAssetBackend.h"
#include "PngTextureLoader.h"
#include "GameCore/Service/Asset/IAssetLoaders.h"

STBAssetBackend::~STBAssetBackend() = default;
STBAssetBackend::STBAssetBackend() = default;

shared_ptr<IAssetLoader> STBAssetBackend::GetLoaderForExtension(string_view ext)
{
	if(ext == ".png") return make_shared<PngTextureLoader>();

	return nullptr;
}

///////////////////////////////////////////////////

unique_ptr<IAssetBackend> CreateSTBAssetBackend()
{
	return make_unique<STBAssetBackend>();
}

