#pragma once
#include "../ResourceProvider.h"

struct TextureCubeAsset;
class EnvironmentResource;
class TextureCubeProvider;
class TextureCubeResource;

class EnvironmentProvider : public ResourceProvider
{
public:
    ~EnvironmentProvider();
    EnvironmentProvider(TaskScheduler& taskScheduler, TextureCubeProvider& cubeProvider) noexcept;
    virtual std::shared_ptr<IResource> CreateResource(std::shared_ptr<AssetData> asset) override;
    void Update();

private:
    std::shared_ptr<TextureCubeResource> CreateCubeResource(std::shared_ptr<TextureCubeAsset> cubeAsset);

    TextureCubeProvider& m_cubeProvider;
};