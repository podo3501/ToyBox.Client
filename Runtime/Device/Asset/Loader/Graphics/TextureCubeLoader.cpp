#include "pch.h"
#include "TextureCubeLoader.h"
#define KHRONOS_STATIC
#include <ktx.h>

PixelFormat VkFormatToPixelFormat(uint32_t vkFormat)
{
    switch (vkFormat)
    {
    case 122: return PixelFormat::R11G11B10F; // VK_FORMAT_B10G11R11_UFLOAT_PACK32 (실제 ktx info 값으로 확인 필요)
    case 97:  return PixelFormat::RGBA16F;    // VK_FORMAT_R16G16B16A16_SFLOAT
    case 43: return PixelFormat::RGBA8;   // VK_FORMAT_R8G8B8A8_UNORM
    default:
        Assert(false);
        return PixelFormat::Unknown;
    }
}

struct KtxTexture2Deleter
{
    void operator()(ktxTexture2* tex) const { ktxTexture_Destroy(ktxTexture(tex)); }
};
using KtxTexture2Ptr = std::unique_ptr<ktxTexture2, KtxTexture2Deleter>;

std::shared_ptr<TextureCubeAsset> TextureCubeLoader::LoadFromMemory(Core::ByteBuffer buffer)
{
    ktxTexture2* rawTex = nullptr;
    KTX_error_code result = ktxTexture2_CreateFromMemory(
        reinterpret_cast<const ktx_uint8_t*>(buffer.data()),
        buffer.size(),
        KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,
        &rawTex);

    if (result != KTX_SUCCESS || !rawTex)
        return nullptr;

    KtxTexture2Ptr ktxTex(rawTex);

    if (ktxTexture2_NeedsTranscoding(ktxTex.get()))
    {
        result = ktxTexture2_TranscodeBasis(ktxTex.get(), KTX_TTF_BC7_RGBA, 0);
        if (result != KTX_SUCCESS)
            return nullptr;
    }

    if (!ktxTex->isCubemap)
    {
        Assert(false); // 이 로더는 큐브맵 전용
        return nullptr;
    }

    auto asset = std::make_shared<TextureCubeAsset>();
    asset->format = VkFormatToPixelFormat(ktxTex->vkFormat);
    asset->colorSpace = ColorSpace::Linear; // cmgen 출력(skybox/IBL 모두)은 항상 Linear
    asset->width = ktxTex->baseWidth;
    asset->height = ktxTex->baseHeight;
    asset->mipCount = ktxTex->numLevels;
    asset->faceCount = ktxTex->numFaces; // 6
    asset->subImages.resize(static_cast<size_t>(asset->mipCount) * asset->faceCount);

    for (uint32_t face = 0; face < asset->faceCount; ++face)
    {
        for (uint32_t mip = 0; mip < asset->mipCount; ++mip)
        {
            ktx_size_t offset = 0;
            KTX_error_code offsetResult = ktxTexture_GetImageOffset(
                ktxTexture(ktxTex.get()), mip, /*layer*/ 0, face, &offset);
            Assert(offsetResult == KTX_SUCCESS);

            ktx_size_t imageSize = ktxTexture_GetImageSize(ktxTexture(ktxTex.get()), mip);
            const uint8_t* srcData = ktxTexture_GetData(ktxTexture(ktxTex.get())) + offset;

            TextureCubeMipFace& sub = asset->subImages[mip + face * asset->mipCount];
            sub.width = std::max(1u, asset->width >> mip);
            sub.height = std::max(1u, asset->height >> mip);
            sub.rowPitch = static_cast<uint32_t>(imageSize / sub.height); // 주의: BC6H 등 블록압축 포맷이면 아래 계산은 틀림. 블록 크기(보통 4x4=16byte) 기준으로 다시 계산해야 함.
            sub.pixels.assign(srcData, srcData + imageSize);
        }
    }

    return asset;
}

unique_ptr<IAssetLoader> CreateTextureCubeLoader()
{
    return make_unique<TextureCubeLoader>();
}