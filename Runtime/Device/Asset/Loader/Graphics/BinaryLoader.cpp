#include "pch.h"
#include "BinaryLoader.h"

std::shared_ptr<BinaryAsset> BinaryLoader::LoadFromMemory(Core::ByteBuffer buffer)
{
    auto asset = std::make_shared<BinaryAsset>();
    asset->buffer = std::move(buffer);

    return asset;
}

unique_ptr<IAssetLoader> CreateBinaryLoader()
{
    return make_unique<BinaryLoader>();
}
