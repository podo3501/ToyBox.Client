#include "pch.h"
#include "TextureProvider.h"
#include "Factory/DescriptorFactory.h"
#include "Resource/Texture/TextureResource.h"
#include "Core/Foundation/Color.h"

TextureProvider::~TextureProvider() = default;
TextureProvider::TextureProvider(TextureCreateGraphBuilder create) noexcept :
    m_createBuilder{ std::move(create) }
{}

bool TextureProvider::Initialize(ShaderLibrary& shaderLibrary)
{
    ReturnIfFalse(CreateBuiltinTextures());
    ReturnIfFalse(m_createBuilder.Initialize(shaderLibrary));

    return true;
}

shared_ptr<TextureResource> TextureProvider::CreateResource(const TextureDesc& desc)
{
    return make_shared<TextureResource>(desc);
}

std::shared_ptr<TextureAsset> TextureProvider::CreateColorAsset(const Core::Color& color)
{
    auto asset = std::make_shared<TextureAsset>();
    asset->size = ToSize(1, 1);
    asset->pixels.resize(sizeof(uint32_t));

    // 비트 연산을 통해 엔디안에 무관하게 항상 R, G, B, A 순서로 메모리 배치
    asset->pixels[0] = color.R8();
    asset->pixels[1] = color.G8();
    asset->pixels[2] = color.B8();
    asset->pixels[3] = color.A8();

    return asset;
}

bool TextureProvider::CreateBuiltinTextures()
{
    std::array<std::shared_ptr<TextureAsset>, Core::EnumSize<BuiltinTextureType>> builtinAssets;
    
    builtinAssets[Core::ToIndex(BuiltinTextureType::White)] = CreateColorAsset(Core::Color::White); // 흰색
    builtinAssets[Core::ToIndex(BuiltinTextureType::FlatNormal)] = CreateColorAsset(Core::Color(0.5f, 0.5f, 1.f)); // 평평한 노멀 (128, 128, 255)
    builtinAssets[Core::ToIndex(BuiltinTextureType::DefaultARM)] = CreateColorAsset(Core::Color(1.f, 0.5f, 0.f)); //ARM 기본

    struct BuiltinConfig { BuiltinTextureType builtinType; const char* name; TextureType texType; };
    BuiltinConfig configs[] = {
        { BuiltinTextureType::White, "BuiltinTexture_White", TextureType::Color },
        { BuiltinTextureType::FlatNormal, "BuiltinTexture_FlatNormal", TextureType::Linear },
        { BuiltinTextureType::DefaultARM, "BuiltinTexture_DefaultARM", TextureType::Linear }
    };

    for (const auto& config : configs)
    {
        TextureDesc desc{ Core::ResourceID::MakeBuiltin(config.name), config.texType, false };

        auto nType = Core::ToIndex(config.builtinType);
        auto tex = CreateBuiltinTexture(desc, builtinAssets[nType]);
        if (!tex) return false;

        m_builtinTextures[nType] = tex;
    }

    return true;
}

std::shared_ptr<TextureResource> TextureProvider::CreateBuiltinTexture(
    const TextureDesc& desc,
    std::shared_ptr<TextureAsset> asset)
{
    auto texRes = CreateResource(desc);
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
    if (!desc.generateMips)
        return baseBytes;

    return static_cast<size_t>(baseBytes * 4 / 3); //mip 비용은 정확 계산 대신 안정적인 근사 (1.33x)
}

bool TextureProvider::LoadResource(
    std::shared_ptr<TextureResource> resource, 
    std::shared_ptr<TextureAsset> asset)
{
    if (!asset) return false;

    auto res = std::static_pointer_cast<TextureResource>(resource);

    TextureLoadRequest req;
    req.resource = res;
    req.asset = asset;
    req.estimatedBytes = EstimateBytes(*asset, res->GetDesc());

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

        if (usedBytes + req.estimatedBytes > uploadBudgetBytes && !batch.empty()) // 최소 1개의 요청은 항상 처리한다. 그렇지 않으면 budget보다 큰 텍스처가 영원히 대기열에 남을 수 있다.
            break; 

        usedBytes += req.estimatedBytes;
        batch.push_back(req);
        m_pending.pop();
    }

    if (batch.empty())
        return;

    m_createBuilder.LoadTextures(batch);
}