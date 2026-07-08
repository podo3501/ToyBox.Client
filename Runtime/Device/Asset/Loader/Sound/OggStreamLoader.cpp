#include "pch.h"
#include "OggStreamLoader.h"
#include "Platform/Resource/IResourceStream.h"

std::shared_ptr<AssetData> OggStreamLoader::Load(AssetInput& source)
{
    if (!source.IsStream()) return nullptr;

    auto& streamInput = static_cast<StreamInput&>(source);
    return LoadFromStream(std::move(streamInput.stream));
}

std::shared_ptr<StreamSoundAsset> OggStreamLoader::LoadFromStream(unique_ptr<IResourceStream> stream)
{
	auto asset = std::make_shared<StreamSoundAsset>();
	asset->stream = move(stream);

	return asset;
}

///////////////////////////////////////////////////////

unique_ptr<IAssetLoader> CreateOggStreamLoader()
{
	return make_unique<OggStreamLoader>();
}