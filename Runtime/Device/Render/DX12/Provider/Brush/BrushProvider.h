#pragma once
#include "GameClient/Service/Render/Repository/Brush/IBrushProvider.h"
#include "../ResourceReleaseBuilder.h"

class BrushResource;
class TextureProvider;

class BrushProvider : public IBrushProvider
{
public:
    ~BrushProvider();
    BrushProvider(TextureProvider& texProvider, ResourceReleaseBuilder release) noexcept;

    std::shared_ptr<IBrushResource> CreateResource() override;
    virtual bool LoadResource(std::shared_ptr<IBrushResource> res, std::shared_ptr<TextureAsset> asset) override;
    virtual void ReleaseResource(std::shared_ptr<IBrushResource> res) override;
    void Update();

private:
    void FlushPendingBrushes();
    void FlushPendingRelease();

    TextureProvider& m_texProvider;
    ResourceReleaseBuilder m_releaseBuilder;

    std::vector<std::shared_ptr<BrushResource>> m_pendingBrushes;
    std::vector<std::shared_ptr<IBrushResource>> m_pendingReleases;
};