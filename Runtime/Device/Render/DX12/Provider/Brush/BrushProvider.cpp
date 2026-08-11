#include "pch.h"
#include "BrushProvider.h"
#include "Resource/Brush/BrushResource.h"
#include "../Texture/TextureProvider.h"

BrushProvider::~BrushProvider() = default;
BrushProvider::BrushProvider(TaskScheduler& taskScheduler, TextureProvider& texProvider) noexcept :
    ResourceProvider{ taskScheduler },
    m_texProvider{ texProvider }
{}

std::shared_ptr<IResource> BrushProvider::CreateResource(std::shared_ptr<AssetData> asset)
{
    auto brushRes = std::make_shared<BrushResource>();

    if (asset)
    {
        auto texRes = m_texProvider.CreateResource();
        if (!texRes) return nullptr;

        auto texAsset = std::static_pointer_cast<TextureAsset>(asset);
        if (!m_texProvider.LoadResource(texRes, texAsset))
            return nullptr;

        brushRes->SetTexture(texRes);
    }
    else
    {
        auto tex = m_texProvider.GetBuiltinTexture(BuiltinTextureType::White);
        brushRes->SetTexture(tex);
    }

    m_pendingLoads.push_back(brushRes);
    return brushRes;
}

void BrushProvider::Update()
{
    FlushPendingLoad();
    FlushPendingRelease();
}