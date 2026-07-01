#include "pch.h"
#include "PngTextureLoader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

shared_ptr<TextureAsset> PngTextureLoader::LoadFromMemory(const Core::ByteBuffer& buffer)
{
    int width, height, channels;

    unsigned char* data = stbi_load_from_memory(
        reinterpret_cast<const unsigned char*>(buffer.data()),
        static_cast<int>(buffer.size()),
        &width,
        &height,
        &channels,
        4 // RGBA 강제. 예를들면 jpg는 3채널이라 알파(A)가 없다.
    );

    if (!data)
        return nullptr;

    auto asset = std::make_shared<TextureAsset>();
    asset->width = width;
    asset->height = height;
    asset->stride = static_cast<uint32_t>(width * 4);
    asset->format = PixelFormat::RGBA8;

    size_t size = width * height * 4;
    asset->pixels.assign(data, data + size);

    stbi_image_free(data);

    return asset;
}

unique_ptr<IAssetLoader> CreateImageTextureLoader()
{
    return make_unique<PngTextureLoader>();
}

std::vector<std::string> GetImageSupportedExtensions()
{
    return {
        ".png",
        ".jpg",
        ".jpeg",
        ".bmp",
        ".tga"
    };
}
