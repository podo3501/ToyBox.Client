#pragma once
#include "GameClient/Service/Render/Repository/IResourceProvider.h"
#include "../IUpdatableProvider.h"

class TextureProvider;
class PendingLoadQueue;
class PendingReleaseQueue;

class BrushProvider : public IResourceProvider
{
public:
    ~BrushProvider();
    BrushProvider() = delete;
    BrushProvider(
        PendingLoadQueue& pendingLoad,
        PendingReleaseQueue& pendingRelease, 
        TextureProvider& texProvider) noexcept;
    virtual std::shared_ptr<IResource> CreateResource(std::shared_ptr<AssetData> asset) override;
    virtual void ReleaseResource(std::shared_ptr<IResource> res) override;

private:
    PendingLoadQueue& m_pendingLoad;
    PendingReleaseQueue& m_pendingRelease;
    TextureProvider& m_texProvider;
};