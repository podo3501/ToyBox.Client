#include "pch.h"
#include "OggStreamLoader.h"
#include "GameClient/Service/Asset/Assets/StreamSoundAsset.h"
#include "Platform/Resource/IResourceStream.h"

std::shared_ptr<Asset> OggStreamLoader::Load(const AssetInput& source)
{
    if (!source.IsStream()) return nullptr;

    const StreamInput* streamInput = static_cast<const StreamInput*>(&source);
    return LoadFromStream(std::move(const_cast<StreamInput*>(streamInput)->stream));
}

std::shared_ptr<Asset> OggStreamLoader::LoadFromStream(unique_ptr<IResourceStream> stream)
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