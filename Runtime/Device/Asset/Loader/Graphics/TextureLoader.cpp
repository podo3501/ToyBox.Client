#include "pch.h"
#include "TextureLoader.h"
#include "Core/Foundation/Cast.hpp"
#include "GameClient/IAssetMetaRegistry.h"
#include "GameClient/Asset/TextureMetaAsset.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static void ApplyPremultipliedAlpha(std::vector<uint8_t>& pixels)
{
    for (size_t i = 0; i + 3 < pixels.size(); i += 4)
    {
        const float a = pixels[i + 3] / 255.0f;
        pixels[i + 0] = static_cast<uint8_t>(pixels[i + 0] * a + 0.5f);
        pixels[i + 1] = static_cast<uint8_t>(pixels[i + 1] * a + 0.5f);
        pixels[i + 2] = static_cast<uint8_t>(pixels[i + 2] * a + 0.5f);
    }
}

TextureLoader::TextureLoader(IAssetMetaRegistry* metaRegistry) noexcept :
    m_metaRegistry{ metaRegistry }
{}

std::shared_ptr<TextureAsset> TextureLoader::LoadFromMemory(const Core::ResourceID& resID, Core::ByteBuffer buffer)
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
    asset->size = ToSize(width, height);
    asset->stride = static_cast<uint32_t>(width * 4);
    asset->format = PixelFormat::RGBA8;

    size_t size = width * height * 4;
    asset->pixels.assign(data, data + size);

    stbi_image_free(data);

    AlphaSourceState source = AlphaSourceState::Straight;
    BlendTargetSpace target = BlendTargetSpace::NonPremultiplied;

    if (m_metaRegistry)
    {
        auto metaData = m_metaRegistry->GetMeta(resID);
        if (auto textureMeta = Core::Cast<TextureMetaAsset>(metaData))
        {
            asset->colorSpace = textureMeta->colorSpace;
            asset->generateMipmaps = textureMeta->generateMipmaps;

            source = textureMeta->alphaSourceState;
            target = textureMeta->blendTargetSpace;
        }
    }

    bool resultIsPMA = false;
    if (source == AlphaSourceState::Opaque || target == BlendTargetSpace::None)
    {
        resultIsPMA = false;
    }
    else if (source == AlphaSourceState::Straight && target == BlendTargetSpace::Premultiplied)
    {
        ApplyPremultipliedAlpha(asset->pixels);
        resultIsPMA = true;
    }
    else if (source == AlphaSourceState::AlreadyPremultiplied && target == BlendTargetSpace::NonPremultiplied)
    {
        Assert(false); // 역변환 케이스. 정밀도 손실이 있을 수 있어 가능하면 임포트 단계에서 이런 조합이 안 나오게 막는 걸 권장.
        resultIsPMA = false;
    }
    else
    {
        // Straight + NonPremultiplied, AlreadyPremultiplied + Premultiplied
        // -> 이미 target과 일치하므로 변환 없음
        resultIsPMA = (target == BlendTargetSpace::Premultiplied);
    }

    asset->isPremultipliedAlpha = resultIsPMA;
    return asset;
}

unique_ptr<IAssetLoader> CreateImageTextureLoader(IAssetMetaRegistry* metaRegistry)
{
    return make_unique<TextureLoader>(metaRegistry);
}
