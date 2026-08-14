#pragma once
#include "../IResourceRepository.h"
#include "ResourceRepositoryImpl.h"

template <typename Tag>
class ResourceRepository : public IResourceRepository
{
public:
    using HandleT = IDHandle<Tag>;

    ResourceRepository(IResourceProvider* provider, IAssetAsyncLoader* asyncLoader)
        : m_impl{ provider, asyncLoader } {}

    HandleT Acquire(const ResourceDesc& desc)
    {
        return HandleCast<Tag>(m_impl.Acquire(desc));
    }

    HandleT AcquireFromAsset(const ResourceDesc& desc, std::shared_ptr<AssetData> asset)
    {
        return HandleCast<Tag>(m_impl.AcquireFromAsset(desc, asset));
    }

    bool Release(HandleT handle)
    {
        return m_impl.Release(HandleCast<ResourceImplTag>(handle));
    }

    std::shared_ptr<IResource> GetIfReady(HandleT handle) const
    {
        return m_impl.GetIfReady(HandleCast<ResourceImplTag>(handle));
    }

    void ReleaseAll() override { m_impl.ReleaseAll(); }
    void Update() override { m_impl.Update(); }

private:
    ResourceRepositoryImpl m_impl;
};