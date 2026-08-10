#pragma once
#include "../ResourceProvider.h"

class BrushResource;
class TextureProvider;
class IPendingResource;

class BrushProvider : public ResourceProvider
{
public:
    ~BrushProvider();
    BrushProvider(TextureProvider& texProvider, TaskScheduler& taskScheduler) noexcept;
    virtual std::shared_ptr<IResource> CreateResource(std::shared_ptr<AssetData> asset) override;

private:
    TextureProvider& m_texProvider;
};