#pragma once
#include "GameCore/Service/Asset/IAssetLoader.h"

class OggStreamLoader : public IAssetLoader
{
public:
    virtual bool PreferStream() const override { return true; }
    virtual std::shared_ptr<Asset> Load(const AssetInput& source) override;

private:
    std::shared_ptr<Asset> LoadFromStream(unique_ptr<IResourceStream> stream);
};

