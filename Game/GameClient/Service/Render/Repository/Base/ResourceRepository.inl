#pragma once

template <typename Traits>
ResourceRepository<Traits>::ResourceRepository(IResourceProvider* provider, IAssetAsyncLoader* asyncLoader) :
    m_provider{ provider },
    m_asyncLoader{ asyncLoader }
{}

template <typename Traits>
std::pair<typename ResourceRepository<Traits>::HandleT, bool>
ResourceRepository<Traits>::FindOrRegister(const DescT& desc)
{
    const size_t key = desc.GetHash();
    auto it = m_cache.find(key);
    if (it != m_cache.end())
        return { it->second, false }; // 이미 존재

    ResourceEntry entry;
    entry.key = key;
    auto handle = m_loadedResources.Emplace(std::move(entry));
    m_cache[key] = handle;

    return { handle, true }; // 새로 등록됨
}

template <typename Traits>
ResourceRepository<Traits>::HandleT 
ResourceRepository<Traits>::Acquire(const DescT& desc)
{
    assert(desc.resID.GetType() == Core::ResourceIDType::Path);

    auto [handle, isNew] = FindOrRegister(desc);
    if (!isNew)
        return handle;

    auto reqID = Asset::PushRequest<AssetT>(m_asyncLoader, desc.resID);
    m_assetPending.push_back({ handle, reqID });
    return handle;
}

template <typename Traits>
ResourceRepository<Traits>::HandleT
ResourceRepository<Traits>::AcquireFromAsset(const DescT& desc, std::shared_ptr<AssetT> asset)
{
    assert(desc.resID.GetType() != Core::ResourceIDType::Path);

    auto [handle, isNew] = FindOrRegister(desc);
    if (!isNew)
        return handle;

    m_resourcePending.push_back({ handle, asset });
    return handle;
}

template <typename Traits>
bool ResourceRepository<Traits>::Release(HandleT handle)
{
    auto entry = m_loadedResources.Find(handle);
    if (!entry) return false;

    m_cache.erase(entry->key);
    std::erase(m_loadingList, handle);

    m_provider->ReleaseResource(std::move(entry->res));
    return m_loadedResources.Remove(handle);
}

template <typename Traits>
void ResourceRepository<Traits>::ReleaseAll()
{
    m_loadedResources.Visit([this](HandleT h, ResourceEntry&) {
        Release(h);
        });

    m_assetPending.clear();
    m_resourcePending.clear();

    m_loadingList.clear();
    m_cache.clear();
    m_loadedResources.Clear();
}

template <typename Traits>
void ResourceRepository<Traits>::Update()
{
    ProcessAssetPending();
    ProcessResourcePending();
    ProcessLoading();
}

template <typename Traits>
std::shared_ptr<IResource> ResourceRepository<Traits>::GetIfReady(HandleT handle) const
{
    auto entry = m_loadedResources.Find(handle);
    if (entry && entry->state == LoadState::Ready)
        return entry->res;

    return nullptr;
}

// ---- 파이프라인 단계별 처리 ----

template <typename Traits>
void ResourceRepository<Traits>::ProcessAssetPending()
{
    if (m_assetPending.empty())
        return;

    for (auto it = m_assetPending.begin(); it != m_assetPending.end();)
    {
        auto& req = *it;
        auto asset = m_asyncLoader->TakeResult(req.requestId);
        if (!asset)
        {
            ++it;
            continue;
        }

        GpuPendingRequest gpuReq;
        gpuReq.handle = req.handle;
        gpuReq.asset = std::static_pointer_cast<AssetT>(asset);
        m_resourcePending.push_back(std::move(gpuReq));

        it = m_assetPending.erase(it);
    }
}

template <typename Traits>
void ResourceRepository<Traits>::ProcessResourcePending()
{
    for (auto& work : m_resourcePending)
    {
        auto entry = m_loadedResources.Find(work.handle);
        if (!entry) continue;
        //if (entry->state != LoadState::Pending) continue; // 중복으로 들어온 경우 이미 Loading/Ready 라면 처리 안 함.

        auto res = m_provider->CreateResource(work.asset);
        if (!res)
        {
            Assert(false); //로딩하다가 실패함.
            entry->state = LoadState::Failed;
            continue;
        }

        entry->res = std::move(res);
        entry->state = LoadState::ResourceLoading;
        m_loadingList.push_back(work.handle);
    }

    m_resourcePending.clear();
}

template <typename Traits>
void ResourceRepository<Traits>::ProcessLoading()
{
    for (auto it = m_loadingList.begin(); it != m_loadingList.end(); )
    {
        auto entry = m_loadedResources.Find(*it);
        if (!entry || !entry->res)
        {
            it = m_loadingList.erase(it);
            continue;
        }

        if (entry->res->IsReady())
        {
            entry->state = LoadState::Ready;
            it = m_loadingList.erase(it);
        }
        else
            ++it;
    }
}
