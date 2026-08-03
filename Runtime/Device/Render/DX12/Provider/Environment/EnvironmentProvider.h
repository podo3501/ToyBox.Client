#pragma once
#include "GameClient/Service/Render/Repository/Environment/IEnvironmentProvider.h"

class EnvironmentProvider : public IEnvironmentProvider
{
public:
    ~EnvironmentProvider();
    EnvironmentProvider() noexcept;
    virtual std::shared_ptr<IEnvironmentResource> CreateResource() override;
    virtual bool LoadResource(std::shared_ptr<IEnvironmentResource> resource, std::shared_ptr<EnvironmentAsset> asset) override;
    virtual void ReleaseResource(std::shared_ptr<IEnvironmentResource> resource) override;

private:
};