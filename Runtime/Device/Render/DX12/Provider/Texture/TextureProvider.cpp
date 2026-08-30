#include "pch.h"
#include "TextureProvider.h"
#include "../ProviderBudget.h"
#include "Factory/DescriptorFactory.h"
#include "Resource/Texture/TextureResource.h"
#include "Core/Foundation/Color.h"

TextureProvider::~TextureProvider() = default;
TextureProvider::TextureProvider(
    Device& device,
    TaskScheduler& taskScheduler,
    ResourceFactory& resFactory,
    DescriptorFactory& descFactory) noexcept :
    m_createBuilder{ device, taskScheduler, resFactory, descFactory }
{}

bool TextureProvider::Initialize(ShaderLibrary& shaderLibrary)
{
    ReturnIfFalse(CreateBuiltinTextures());
    ReturnIfFalse(m_createBuilder.Initialize(shaderLibrary));

    return true;
}

shared_ptr<TextureResource> TextureProvider::CreateResource()
{
    return make_shared<TextureResource>();
}

static std::shared_ptr<TextureAsset> CreateColorAsset(const Core::Color& color, ColorSpace colorSpace)
{
    auto asset = std::make_shared<TextureAsset>();
    asset->size = ToSize(1, 1);
    asset->pixels.resize(sizeof(uint32_t));

    // 비트 연산을 통해 엔디안에 무관하게 항상 R, G, B, A 순서로 메모리 배치
    asset->pixels[0] = color.R8();
    asset->pixels[1] = color.G8();
    asset->pixels[2] = color.B8();
    asset->pixels[3] = color.A8();

    asset->colorSpace = colorSpace;
    asset->generateMipmaps = false; //같은 색상이기 때문에 불필요
    asset->isPremultipliedAlpha = false; //ui도 기본은 false로.

    return asset;
}

bool TextureProvider::CreateBuiltinTextures()
{
    std::array<std::shared_ptr<TextureAsset>, Core::EnumSize<BuiltinTextureType>> builtinAssets;
    
    builtinAssets[Core::ToIndex(BuiltinTextureType::White)] = CreateColorAsset(Core::Color::White, ColorSpace::SRGB); // 흰색
    builtinAssets[Core::ToIndex(BuiltinTextureType::FlatNormal)] = CreateColorAsset(Core::Color(0.5f, 0.5f, 1.f), ColorSpace::Linear); // 평평한 노멀 (128, 128, 255)
    builtinAssets[Core::ToIndex(BuiltinTextureType::DefaultARM)] = CreateColorAsset(Core::Color(1.f, 0.3f, 0.5f), ColorSpace::Linear); //ARM 기본
    
    for (int nType = 0; nType < Core::EnumSize<BuiltinTextureType>; nType++)
    {
        auto tex = CreateBuiltinTexture(builtinAssets[nType]);
        if (!tex) return false;

        m_builtinTextures[nType] = tex;
    }

    return true;
}

std::shared_ptr<TextureResource> TextureProvider::CreateBuiltinTexture(std::shared_ptr<TextureAsset> asset)
{
    auto texRes = CreateResource();
    if (!texRes)
        return nullptr;

    if (!LoadResource(texRes, asset))
        return nullptr;

    return texRes;
}

std::shared_ptr<TextureResource> TextureProvider::GetBuiltinTexture(BuiltinTextureType type) const
{
    auto idx = Core::ToIndex(type);
    Assert(idx < m_builtinTextures.size());

    return m_builtinTextures[idx];
}

static size_t EstimateBytes(const TextureAsset& asset, const TextureDesc& desc)
{
    size_t baseBytes = asset.pixels.size();
    if (!desc.generateMipmaps)
        return baseBytes;

    return static_cast<size_t>(baseBytes * 4 / 3); //mip 비용은 정확 계산 대신 안정적인 근사 (1.33x)
}

bool TextureProvider::LoadResource(
    std::shared_ptr<TextureResource> resource, 
    std::shared_ptr<TextureAsset> asset)
{
    if (!asset) return false;

    TextureDesc desc{ asset->colorSpace, asset->generateMipmaps, asset->isPremultipliedAlpha };
    resource->SetDesc(desc);

    TextureLoadRequest req;
    req.resource = resource;
    req.asset = asset;
    req.estimatedBytes = EstimateBytes(*asset, resource->GetDesc());

    m_pendingLoads.Push(req);
    return true;
}

void TextureProvider::Update(float avgGpuMs)
{
    auto uploadBudgetBytes = ComputeBudget(avgGpuMs, ProviderBudget::Texture);

    m_pendingLoads.Flush(uploadBudgetBytes, [this](std::vector<TextureLoadRequest>& batch) {
        m_createBuilder.LoadTextures(batch);
        });
}