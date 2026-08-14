#pragma once

#include "Core/Utils/Handle/HandlePool.h"
#include "ResourceTypes.h"
#include "Service/IAssetAsyncLoader.h"
#include "Service/Render/Definition/ResourceDesc.h"

struct IResourceProvider;
struct IResource;

struct ResourceEntry
{
    size_t key{};
    std::shared_ptr<IResource> res{ nullptr };
    LoadState state{ LoadState::Pending };
};

struct ResourceImplTag {}; // 내부 전용 Tag - 외부 Tag와 무관

class ResourceRepositoryImpl
{
public:
    using RawHandle = IDHandle<ResourceImplTag>;

    ~ResourceRepositoryImpl();
    ResourceRepositoryImpl(IResourceProvider* provider, IAssetAsyncLoader* asyncLoader);

    void ReleaseAll();
    void Update();

    RawHandle Acquire(const ResourceDesc& desc);
    RawHandle AcquireFromAsset(const ResourceDesc& desc, std::shared_ptr<AssetData> asset);
    bool Release(RawHandle handle);
    std::shared_ptr<IResource> GetIfReady(RawHandle handle) const;

private:
    struct CpuPendingRequest { RawHandle handle; AssetRequestID requestId; };
    struct GpuPendingRequest { RawHandle handle; std::shared_ptr<AssetData> asset; };

    std::pair<RawHandle, bool> FindOrRegister(size_t key);
    void ProcessAssetPending();
    void ProcessResourcePending();
    void ProcessLoading();

private:
    IResourceProvider* m_provider;
    IAssetAsyncLoader* m_asyncLoader;

    std::unordered_map<size_t, RawHandle> m_cache;
    HandlePool<ResourceEntry, ResourceImplTag> m_loadedResources;

    std::vector<CpuPendingRequest> m_assetPending;
    std::vector<GpuPendingRequest> m_resourcePending;
    std::vector<RawHandle> m_loadingList;
};