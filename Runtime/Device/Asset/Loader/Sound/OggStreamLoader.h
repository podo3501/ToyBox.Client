#pragma once
#include "GameClient/Asset/IAssetLoader.h"
#include "GameClient/Asset/StreamSoundAsset.h"

class OggStreamLoader : public IAssetLoader
{
public:
    virtual bool PreferStream() const override { return true; }
    virtual std::shared_ptr<AssetData> Load(const AssetInput& source) override;

private:
    std::shared_ptr<StreamSoundAsset> LoadFromStream(unique_ptr<IResourceStream> stream);
};

