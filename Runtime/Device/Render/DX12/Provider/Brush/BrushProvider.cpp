#include "pch.h"
#include "BrushProvider.h"
#include "Resource/Brush/BrushResource.h"
#include "../Texture/TextureProvider.h"

BrushProvider::~BrushProvider() = default;
BrushProvider::BrushProvider(TextureProvider& texProvider, ResourceReleaseBuilder release) noexcept :
    m_texProvider{ texProvider },
    m_releaseBuilder{ std::move(release) }
{}

std::shared_ptr<IBrushResource> BrushProvider::CreateResource()
{
    auto brushRes = make_shared<BrushResource>();

    auto tex = m_texProvider.GetBuiltinTexture(BuiltinTextureType::White);
    brushRes->SetTexture(tex);

    return brushRes;
}

bool BrushProvider::LoadResource(std::shared_ptr<IBrushResource> res, std::shared_ptr<TextureAsset> asset)
{
    if (!asset)
        return false;

    auto brushRes = std::static_pointer_cast<BrushResource>(res);
    if (!brushRes) return false;

    auto texRes = m_texProvider.CreateResource();
    if (!texRes) return false;

    if (!m_texProvider.LoadResource(texRes, asset))
        return false;

    brushRes->SetTexture(texRes);
    m_pendingBrushes.push_back(brushRes);

    return true;
}

void BrushProvider::ReleaseResource(std::shared_ptr<IBrushResource> resource)
{
    if (!resource)
        return;

    m_pendingReleases.emplace_back(std::move(resource));
}

void BrushProvider::Update()
{
    FlushPendingBrushes();
    FlushPendingRelease();
}

void BrushProvider::FlushPendingBrushes()
{
    for (auto it = m_pendingBrushes.begin(); it != m_pendingBrushes.end();)
    {
        auto& brushRes = *it;
        if (!brushRes->IsTextureReady())
        {
            ++it;
            continue;
        }
        brushRes->MarkReady();
        it = m_pendingBrushes.erase(it);
    }
}

void BrushProvider::FlushPendingRelease()
{
    if (m_pendingReleases.empty())
        return;

    m_releaseBuilder.ReleaseResources(std::move(m_pendingReleases));
}