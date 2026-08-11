#pragma once
#include "../ResourceProvider.h"

class BrushResource;
class TextureProvider;
class IPendingResource;

class BrushProvider : public ResourceProvider
{
public:
    ~BrushProvider();
    BrushProvider(TaskScheduler& taskScheduler, TextureProvider& texProvider) noexcept;
    virtual std::shared_ptr<IResource> CreateResource(std::shared_ptr<AssetData> asset) override;
    void Update();

private:
    TextureProvider& m_texProvider;
};