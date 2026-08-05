#pragma once
#include "GameClient/Asset/IAssetLoader.h"
#include "GameClient/Asset/IAssetProvider.h"
#include "GameClient/Asset/MeshAsset.h"

class MeshGltfLoader : public IAssetLoader
{
public:
    explicit MeshGltfLoader(IAssetProvider* assetProvider) noexcept;
    virtual std::shared_ptr<AssetData> Load(AssetInput& source) override
    {
        if (source.IsStream()) return nullptr;

        auto& mem = static_cast<MemoryInput&>(source);
        return LoadFromMemory(mem.resID, std::move(mem.buffer));
    }

private:
    std::shared_ptr<MeshAsset> LoadFromMemory(const Core::ResourceID& resID, Core::ByteBuffer buffer);

    IAssetProvider* m_assetProvider{ nullptr };
};