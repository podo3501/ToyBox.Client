#pragma once
#include "../IUpdatableProvider.h"
#include "TextureCubeLoadRequest.h"
#include "TextureCubeCreateGraphBuilder.h"
#include "../PendingUploadQueue.h"

struct TextureCubeLoadRequest;
class TextureCubeResource;

class TextureCubeProvider : public IUpdatableProvider
{
public:
    ~TextureCubeProvider();
    TextureCubeProvider(
        TaskScheduler& taskScheduler,
        ResourceFactory& resFactory,
        DescriptorFactory& descFactory) noexcept;
    virtual void Update(float avgGpuMs) override;

    std::shared_ptr<TextureCubeResource> CreateResource();
    bool LoadResource(
        std::shared_ptr<TextureCubeResource> resource,
        std::shared_ptr<TextureCubeAsset> asset);

private:
    TextureCubeCreateGraphBuilder m_createBuilder;
    PendingUploadQueue<TextureCubeLoadRequest> m_pendingLoads;
};