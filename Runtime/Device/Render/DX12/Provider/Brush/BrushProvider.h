#pragma once
#include "GameClient/Service/Render/Repository/IResourceProvider.h"
#include "../IUpdatableProvider.h"
#include "../PendingLoadQueue.h"
#include "../PendingReleaseQueue.h"

class TextureProvider;

class BrushProvider : public IResourceProvider, public IUpdatableProvider
{
public:
    ~BrushProvider();
    BrushProvider() = delete;
    BrushProvider(TaskScheduler& taskScheduler, TextureProvider& texProvider) noexcept;
    virtual std::shared_ptr<IResource> CreateResource(std::shared_ptr<AssetData> asset) override;
    virtual void ReleaseResource(std::shared_ptr<IResource> res) override;
    virtual void Update(float) override;

private:
    PendingLoadQueue m_pendingLoad;
    PendingReleaseQueue m_pendingRelease;
    TextureProvider& m_texProvider;
};