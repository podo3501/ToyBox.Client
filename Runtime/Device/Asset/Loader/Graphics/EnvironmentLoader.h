#pragma once
#include "GameClient/Asset/IAssetLoader.h"
#include "GameClient/Asset/IAssetProvider.h"
#include "GameClient/Asset/EnvironmentAsset.h"

class EnvironmentLoader : public IAssetLoader
{
public:
    explicit EnvironmentLoader(IAssetProvider* assetProvider) noexcept;
    virtual std::shared_ptr<AssetData> Load(AssetInput& source) override
    {
        if (source.IsStream()) return nullptr;

        auto& mem = static_cast<MemoryInput&>(source);
        return LoadFromMemory(mem.resID, std::move(mem.buffer));
    }

private:
    std::shared_ptr<EnvironmentAsset> LoadFromMemory(const Core::ResourceID& resID, Core::ByteBuffer buffer);

    IAssetProvider* m_assetProvider{ nullptr };
};