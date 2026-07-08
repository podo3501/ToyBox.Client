#include "pch.h"
#include "FontLoader.h"

std::shared_ptr<FontAsset> FontLoader::LoadFromMemory(Core::ByteBuffer buffer)
{
    auto asset = std::make_shared<FontAsset>();
    asset->fontSource = std::move(buffer);

    return asset;
}

unique_ptr<IAssetLoader> CreateFontLoader()
{
    return make_unique<FontLoader>();
}
