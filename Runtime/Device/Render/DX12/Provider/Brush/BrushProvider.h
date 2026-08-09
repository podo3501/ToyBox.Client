#pragma once
#include "GameClient/Service/Render/Repository/Provider/IBrushProvider.h"
#include "../ResourceReleaseBuilder.h"

class BrushResource;
class TextureProvider;

class BrushProvider : public IBrushProvider
{
public:
    ~BrushProvider();
    BrushProvider(TextureProvider& texProvider, ResourceReleaseBuilder release) noexcept;

    std::shared_ptr<IResource> CreateResource() override;
    virtual bool LoadResource(std::shared_ptr<IResource> res, std::shared_ptr<TextureAsset> asset) override;
    virtual void ReleaseResource(std::shared_ptr<IResource> res) override;
    void Update();

private:
    void FlushPendingBrushes();
    void FlushPendingRelease();

    TextureProvider& m_texProvider;
    ResourceReleaseBuilder m_releaseBuilder;

    std::vector<std::shared_ptr<BrushResource>> m_pendingBrushes;
    std::vector<std::shared_ptr<IResource>> m_pendingReleases;
};