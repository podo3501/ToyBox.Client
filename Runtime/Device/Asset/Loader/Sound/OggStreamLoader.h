#pragma once
#include "GameCore/Service/Asset/IAssetLoader.h"
#include "GameClient/Service/Asset/Assets/StreamSoundAsset.h"

class OggStreamLoader : public IAssetLoader
{
public:
    virtual bool PreferStream() const override { return true; }
    virtual std::shared_ptr<AssetData> Load(const AssetInput& source) override;

private:
    std::shared_ptr<StreamSoundAsset> LoadFromStream(unique_ptr<IResourceStream> stream);
};

