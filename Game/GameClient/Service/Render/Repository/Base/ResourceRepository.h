#pragma once
#include "../IResourceRepository.h"
#include "../IResourceProvider.h"
#include "Core/Utils/Handle/HandlePool.h"
#include "ResourceTypes.h"
#include "Service/IAssetAsyncLoader.h"
#include "Service/AssetAsyncHelper.h"

struct IResource;

struct ResourceEntry
{
    size_t key{};
    std::shared_ptr<IResource> res{ nullptr };
    LoadState state{ LoadState::Pending };
};

template <typename Traits>
class ResourceRepository : public IResourceRepository
{
public:
    using TagT = typename Traits::Tag;
    using HandleT = IDHandle<TagT>;
    using DescT = typename Traits::Desc;
    using AssetT = typename Traits::Asset;

    ResourceRepository(IResourceProvider* provider, IAssetAsyncLoader* asyncLoader);
    virtual ~ResourceRepository() = default;

    HandleT Acquire(const DescT& desc); //path
    HandleT AcquireFromAsset(const DescT& desc, std::shared_ptr<AssetT> asset);
    bool Release(HandleT handle);
    void ReleaseAll();
    void Update();

    std::shared_ptr<IResource> GetIfReady(HandleT handle) const;

private:
    struct CpuPendingRequest
    {
        HandleT handle;
        AssetRequestID requestId;
    };

    struct GpuPendingRequest
    {
        HandleT handle;
        std::shared_ptr<AssetT> asset;
    };

    HandleT RegisterNewEntry(size_t key);
    void ProcessAssetPending();  // CPU 로드 완료된 항목을 GPU 대기열로 이동
    void ProcessResourcePending();  // GPU 업로드 시작, 완료 대기열(m_loadingList)로 이동
    void ProcessLoading(); // GPU 업로드 완료 여부 확인, 완료 시 Ready 처리

private:
    IResourceProvider* m_provider;
    IAssetAsyncLoader* m_asyncLoader;

    std::unordered_map<size_t, HandleT> m_cache;
    HandlePool<ResourceEntry, TagT> m_loadedResources;

    std::vector<CpuPendingRequest> m_assetPending;
    std::vector<GpuPendingRequest> m_resourcePending;
    std::vector<HandleT> m_loadingList;
};

#include "ResourceRepository.inl"