#include "pch.h"
#include "TextureSystem.h"
#include "MipGenerator.h"
#include "DescriptorFactory.h"
#include "TextureGraphBuilder.h"
#include "TextureResource.h"

TextureSystem::~TextureSystem() = default;
TextureSystem::TextureSystem(ID3D12Device* device, DescriptorAllocator* srvAllocator, TaskScheduler* taskScheduler, ResourceLoader* loader) :
    m_mipGenerator{ make_unique<MipGenerator>(device, srvAllocator) },
    m_descriptorFactory{ make_unique<DescriptorFactory>(device, srvAllocator) },
    m_builder{ make_unique<TextureGraphBuilder>(taskScheduler, loader, 
        m_mipGenerator.get(), m_descriptorFactory.get()) }
{}

bool TextureSystem::Initialize()
{
    ReturnIfFalse(CreateBuiltinTextures());
    ReturnIfFalse(m_mipGenerator->Initialize());

    return true;
}

shared_ptr<ITextureResource> TextureSystem::CreateTextureResource()
{
    return make_shared<TextureResource>();
}

std::shared_ptr<TextureAsset> TextureSystem::CreateDefaultTextureAsset()
{
    auto asset = std::make_shared<TextureAsset>();

    asset->width = 1;
    asset->height = 1;
    asset->pixels.resize(sizeof(uint32_t));

    uint32_t white = 0xffffffff;
    memcpy(asset->pixels.data(), &white, sizeof(uint32_t));

    return asset;
}

bool TextureSystem::CreateBuiltinTextures()
{
    auto texRes = CreateTextureResource();
    if (!texRes)
        return false;

    m_defaultAsset = CreateDefaultTextureAsset();

    TextureDesc desc{ true, false };
    if (!LoadFromAsset(texRes, m_defaultAsset, desc))
        return false;

    m_defaultTexture = texRes;

    return true;
}

static size_t EstimateBytes(const TextureAsset& asset, const TextureDesc& desc)
{
    size_t baseBytes = asset.pixels.size();
    if (!desc.generateMips)
        return baseBytes;

    return static_cast<size_t>(baseBytes * 4 / 3); //mip 비용은 정확 계산 대신 안정적인 근사 (1.33x)
}

bool TextureSystem::LoadFromAsset(
    std::shared_ptr<ITextureResource> resource, 
    std::shared_ptr<TextureAsset> asset,
    const TextureDesc& desc)
{
    if(!asset) asset = m_defaultAsset;

    TextureLoadRequest req;
    req.resource = resource;
    req.asset = asset;
    req.desc = desc;
    req.estimatedBytes = EstimateBytes(*asset, desc);

    m_pending.push(req);
    return true;
}

void TextureSystem::Update(size_t uploadBudgetBytes)
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