#include "pch.h"
#include "EnvironmentProvider.h"

EnvironmentProvider::~EnvironmentProvider() = default;
EnvironmentProvider::EnvironmentProvider() noexcept = default;

std::shared_ptr<IEnvironmentResource> EnvironmentProvider::CreateResource()
{
    return nullptr;
}

bool EnvironmentProvider::LoadResource(std::shared_ptr<IEnvironmentResource> resource, std::shared_ptr<EnvironmentAsset> asset)
{
    return true;
}

void EnvironmentProvider::ReleaseResource(std::shared_ptr<IEnvironmentResource> resource)
{

}