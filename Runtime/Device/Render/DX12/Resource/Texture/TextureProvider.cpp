#include "pch.h"
#include "TextureProvider.h"
#include "MipGenerator.h"
#include "Descriptor/DescriptorFactory.h"
#include "TextureGraphBuilder.h"
#include "TextureResource.h"

TextureProvider::~TextureProvider() = default;
TextureProvider::TextureProvider(Device& device, DescriptorFactory* descFactory, TaskScheduler* taskScheduler, ResourceLoader* loader) :
    m_mipGenerator{ make_unique<MipGenerator>(device) },
    m_builder{ make_unique<TextureGraphBuilder>(taskScheduler, loader, m_mipGenerator.get(), descFactory) }
{}

bool TextureProvider::Initialize(ShaderProvider* shaderProvider)
{
    ReturnIfFalse(CreateBuiltinTextures());
    ReturnIfFalse(m_mipGenerator->Initialize(shaderProvider));

    return true;
}

shared_ptr<ITextureResource> TextureProvider::CreateTextureResource(const TextureDesc& desc)
{
    return make_shared<TextureResource>(desc);
}

std::shared_ptr<TextureAsset> TextureProvider::CreateColorAsset(uint32_t pixelColor)
{
    auto asset = std::make_shared<TextureAsset>();

    asset->width = 1;
    asset->height = 1;
    asset->pixels.resize(sizeof(uint32_t));

    // 비트 연산을 통해 엔디안에 무관하게 항상 R, G, B, A 순서로 메모리 배치
    asset->pixels[0] = static_cast<uint8_t>((pixelColor >> 24) & 0xFF); // R
    asset->pixels[1] = static_cast<uint8_t>((pixelColor >> 16) & 0xFF); // G
    asset->pixels[2] = static_cast<uint8_t>((pixelColor >> 8) & 0xFF); // B
    asset->pixels[3] = static_cast<uint8_t>((pixelColor >> 0) & 0xFF); // A

    return asset;
}

bool TextureProvider::CreateBuiltinTextures()
{
    m_defaultAssets[DefaultTextureType::White] = CreateColorAsset(0xFFFFFFFF); // 흰색
    m_defaultAssets[DefaultTextureType::FlatNormal] = CreateColorAsset(0x8080FFFF); // 평평한 노멀 (128, 128, 255)
    m_defaultAssets[DefaultTextureType::Orange] = CreateColorAsset(0xFF8000FF);

    struct BuiltinConfig { DefaultTextureType type; const char* name; bool srgb; };
    BuiltinConfig configs[] = {
        { DefaultTextureType::White, "DefaultTexture_White", true },
        { DefaultTextureType::FlatNormal, "DefaultTexture_FlatNormal", false },
        { DefaultTextureType::Orange, "DefaultTexture_Orange", false }
    };

    for (const auto& config : configs)
    {
        TextureDesc desc{ Core::ResourceID::MakeBuiltin(config.name), config.srgb, false };
        auto tex = CreateDefaultTexture(desc, m_defaultAssets[config.type]);
        if (!tex) return false;

        m_defaultTextures[config.type] = tex;
    }

    return true;
}

std::shared_ptr<ITextureResource> TextureProvider::CreateDefaultTexture(
    const TextureDesc& desc,
    std::shared_ptr<TextureAsset> asset)
{
    auto texRes = CreateTextureResource(desc);
    if (!texRes)
        return nullptr;

    if (!LoadFromAsset(texRes, asset))
        return nullptr;

    return texRes;
}

std::shared_ptr<ITextureResource> TextureProvider::GetDefaultTexture(DefaultTextureType type) const
{
    auto it = m_defaultTextures.find(type);
    if (it != m_defaultTextures.end())
        return it->second;

    return nullptr; // 혹은 시스템 전체 폴백(Albedo) 리턴
}

static size_t EstimateBytes(const TextureAsset& asset, const TextureDesc& desc)
{
    size_t baseBytes = asset.pixels.size();
    if (!desc.generateMips)
        return baseBytes;

    return static_cast<size_t>(baseBytes * 4 / 3); //mip 비용은 정확 계산 대신 안정적인 근사 (1.33x)
}

bool TextureProvider::LoadFromAsset(
    std::shared_ptr<ITextureResource> resource, 
    std::shared_ptr<TextureAsset> asset)
{
    auto res = std::static_pointer_cast<TextureResource>(resource);
    auto& texDesc = res->GetDesc();

    if(!asset) 
        asset = texDesc.srgb ? m_defaultAssets[DefaultTextureType::White] : m_defaultAssets[DefaultTextureType::FlatNormal]; //?!? 지금은 이렇게 하고 나중에는 이 윗단에서 default를 넣어줘야 한다.
    
    TextureLoadRequest req;
    req.resource = res;
    req.asset = asset;
    req.estimatedBytes = EstimateBytes(*asset, texDesc);

    m_pending.push(req);
    return true;
}

void TextureProvider::Update(size_t uploadBudgetBytes)
{
    size_t usedBytes = 0;
    std::vector<TextureLoadRequest> batch;
    batch.reserve(32);

    while (!m_pending.empty())
    {
        auto& req = m_pending.front();

        if (usedBytes + req.estimatedBytes > uploadBudgetBytes && !batch.empty())
            break;

        usedBytes += req.estimatedBytes;
        batch.push_back(req);
        m_pending.pop();
    }

    if (batch.empty())
        return;

    m_builder->LoadTextures(batch);
}