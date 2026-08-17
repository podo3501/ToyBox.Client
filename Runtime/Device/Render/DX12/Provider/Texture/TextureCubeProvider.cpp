#include "pch.h"
#include "TextureCubeProvider.h"
#include "../ProviderBudget.h"

TextureCubeProvider::~TextureCubeProvider() = default;
TextureCubeProvider::TextureCubeProvider(TextureCubeCreateGraphBuilder create) noexcept :
    m_createBuilder{ std::move(create) }
{}

std::shared_ptr<TextureCubeResource> TextureCubeProvider::CreateResource()
{
    return std::make_shared<TextureCubeResource>();
}

static size_t EstimateBytes(const TextureCubeAsset& asset)
{
    // 밉이 이미 cmgen에서 다 구워져 있으므로 근사치 없이 subImages 실제 바이트 합산
    size_t total = 0;
    for (const auto& sub : asset.subImages)
        total += sub.pixels.size();
    return total;
}

bool TextureCubeProvider::LoadResource(
    std::shared_ptr<TextureCubeResource> resource,
    std::shared_ptr<TextureCubeAsset> asset)
{
    if (!asset) return false;

    TextureCubeDesc desc{ asset->colorSpace }; // 항상 Linear
    resource->SetDesc(desc);

    TextureCubeLoadRequest req;
    req.resource = resource;
    req.asset = asset;
    req.estimatedBytes = EstimateBytes(*asset);

    m_pendingLoads.Push(req);
    return true;
}

void TextureCubeProvider::Update(float avgGpuMs)
{
    auto uploadBudgetBytes = ComputeBudget(avgGpuMs, ProviderBudget::TextureCube);

    m_pendingLoads.Flush(uploadBudgetBytes, [this](std::vector<TextureCubeLoadRequest>& batch) {
        m_createBuilder.LoadTextureCubes(batch);
        });
}