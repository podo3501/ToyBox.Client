#include "pch.h"
#include "BrushProvider.h"
#include "Resource/Brush/BrushResource.h"
#include "../Texture/TextureProvider.h"
#include "Core/Foundation/Cast.hpp"

BrushProvider::~BrushProvider() = default;
BrushProvider::BrushProvider(TaskScheduler& taskScheduler, TextureProvider& texProvider) noexcept :
    m_pendingRelease{ taskScheduler },
    m_texProvider{ texProvider }
{}

std::shared_ptr<IResource> BrushProvider::CreateResource(std::shared_ptr<AssetData> asset)
{
    auto brushRes = std::make_shared<BrushResource>();

    if (asset)
    {
        auto texRes = m_texProvider.CreateResource();
        if (!texRes) return nullptr;

        auto texAsset = Core::Cast<TextureAsset>(asset);
        if (!texAsset) return nullptr;

        if (!m_texProvider.LoadResource(texRes, texAsset))
            return nullptr;

        brushRes->SetTexture(texRes);
    }
    else
    {
        auto tex = m_texProvider.GetBuiltinTexture(BuiltinTextureType::White);
        brushRes->SetTexture(tex);
    }

    m_pendingLoad.Add(brushRes);
    return brushRes;
}

void BrushProvider::ReleaseResource(std::shared_ptr<IResource> res)
{
    m_pendingRelease.Add(std::move(res));
}

void BrushProvider::Update()
{
    m_pendingLoad.Flush();
    m_pendingRelease.Flush();
}