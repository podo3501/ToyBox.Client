#include "pch.h"
#include "KtxTextureLoader.h"
#include "Core/Foundation/Cast.hpp"
#include "GameClient/IAssetMetaRegistry.h"
#include "GameClient/Asset/TextureMetaAsset.h"
#define KHRONOS_STATIC
#include <ktx.h>

static PixelFormat VkFormatToPixelFormat(ktx_uint32_t vkFormat)
{
    switch (vkFormat)
    {
    case 97: return PixelFormat::RGBA16F; // VK_FORMAT_R16G16B16A16_SFLOAT
    case 43: return PixelFormat::RGBA8;   // VK_FORMAT_R8G8B8A8_UNORM
    default:
        Assert(false); // 매핑되지 않은 vkFormat -> 여기 케이스 추가 필요
        return PixelFormat::Unknown;
    }
}

KtxTextureLoader::KtxTextureLoader(IAssetMetaRegistry* metaRegistry) noexcept :
    m_metaRegistry{ metaRegistry }
{}

std::shared_ptr<CubemapAsset> KtxTextureLoader::LoadFromMemory(const Core::ResourceID& resID, Core::ByteBuffer buffer)
{
    ktxTexture2* ktxTex = nullptr;
    KTX_error_code result = ktxTexture2_CreateFromMemory(
        reinterpret_cast<const ktx_uint8_t*>(buffer.data()),
        buffer.size(),
        KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,
        &ktxTex);

    if (result != KTX_SUCCESS || !ktxTex)
        return nullptr;

    // Basis Universal 등으로 supercompress 되어 있으면 실제 GPU 포맷으로 트랜스코딩해야 함
    if (ktxTexture2_NeedsTranscoding(ktxTex))
    {
        result = ktxTexture2_TranscodeBasis(ktxTex, KTX_TTF_BC7_RGBA, 0);
        if (result != KTX_SUCCESS)
        {
            ktxTexture_Destroy(ktxTexture(ktxTex));
            return nullptr;
        }
    }

    if (!ktxTex->isCubemap)
    {
        Assert(false); // 이 로더는 큐브맵(스카이박스/IBL) 전용
        ktxTexture_Destroy(ktxTexture(ktxTex));
        return nullptr;
    }

    auto asset = std::make_shared<CubemapAsset>();
    asset->format = VkFormatToPixelFormat(ktxTex->vkFormat);
    asset->width = ktxTex->baseWidth;
    asset->height = ktxTex->baseHeight;
    asset->mipCount = ktxTex->numLevels;
    asset->faceCount = ktxTex->numFaces; // 6

    if (m_metaRegistry)
    {
        auto metaData = m_metaRegistry->GetMeta(resID);
        if (auto textureMeta = Core::Cast<TextureMetaAsset>(metaData))
        {
            asset->colorSpace = textureMeta->colorSpace;
            // IBL/스카이박스는 cmgen이 밉체인을 이미 구워서 넣어주므로
            // generateMipmaps 같은 런타임 밉 생성 옵션은 여기선 무시
        }
    }

    asset->subImages.resize(asset->mipCount * asset->faceCount);

    for (uint32_t face = 0; face < asset->faceCount; ++face)
    {
        for (uint32_t mip = 0; mip < asset->mipCount; ++mip)
        {
            ktx_size_t offset = 0;
            KTX_error_code offsetResult = ktxTexture_GetImageOffset(
                ktxTexture(ktxTex), mip, /*layer*/ 0, face, &offset);
            Assert(offsetResult == KTX_SUCCESS);

            ktx_size_t imageSize = ktxTexture_GetImageSize(ktxTexture(ktxTex), mip);
            const uint8_t* srcData = ktxTexture_GetData(ktxTexture(ktxTex)) + offset;

            CubemapMipFace& sub = asset->subImages[mip + face * asset->mipCount];
            sub.width = std::max(1u, asset->width >> mip);
            sub.height = std::max(1u, asset->height >> mip);

            // 주의: BC6H 등 블록압축 포맷이면 아래 계산은 틀림. 블록 크기(보통 4x4=16byte) 기준으로 다시 계산해야 함.
            sub.rowPitch = static_cast<uint32_t>(imageSize / sub.height);

            sub.pixels.assign(srcData, srcData + imageSize);
        }
    }

    ktxTexture_Destroy(ktxTexture(ktxTex));
    return asset;
}

unique_ptr<IAssetLoader> CreateKtxTextureLoader(IAssetMetaRegistry* metaRegistry)
{
    return make_unique<KtxTextureLoader>(metaRegistry);
}