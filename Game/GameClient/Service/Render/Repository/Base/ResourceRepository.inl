#pragma once

template <typename Traits>
ResourceRepository<Traits>::ResourceRepository(IResourceProvider* provider, IAssetAsyncLoader* asyncLoader) :
    m_provider{ provider },
    m_asyncLoader{ asyncLoader }
{}

template <typename Traits>
typename ResourceRepository<Traits>::HandleT
ResourceRepository<Traits>::GetOrCreate(const DescT& desc)
{
    const size_t key = desc.GetHash();
    auto it = m_cache.find(key);
    if (it != m_cache.end())
        return it->second;

    ResourceEntry entry;
    entry.key = key;

    auto handle = m_loadedResources.Emplace(std::move(entry));
    m_cache[key] = handle;

    switch (desc.resID.GetType())
    {
    case Core::ResourceIDType::Path:
    {
        auto reqID = Asset::PushRequest<AssetT>(m_asyncLoader, desc.resID);
        m_cpuPending.push_back({ handle, reqID });
    }
    break;
    case Core::ResourceIDType::Runtime:
    case Core::ResourceIDType::Builtin:
        m_gpuPending.push_back({ handle, nullptr });
        break;
    default:
        return HandleT::Invalid();
    }

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

    m_cpuPending.clear();
    m_gpuPending.clear();

    m_loadingList.clear();
    m_cache.clear();
    m_loadedResources.Clear();
}

template <typename Traits>
void ResourceRepository<Traits>::Update()
{
    ProcessCpuPending();
    ProcessGpuPending();
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
void ResourceRepository<Traits>::ProcessCpuPending()
{
    if (m_cpuPending.empty())
        return;

    for (auto it = m_cpuPending.begin(); it != m_cpuPending.end();)
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
        m_gpuPending.push_back(std::move(gpuReq));

        it = m_cpuPending.erase(it);
    }
}

template <typename Traits>
void ResourceRepository<Traits>::ProcessGpuPending()
{
    for (auto& work : m_gpuPending)
    {
        auto entry = m_loadedResources.Find(work.handle);
        if (!entry)
            continue;

        auto res = m_provider->CreateResource(work.asset);
        if (!res)
        {
            entry->state = LoadState::Failed;
            continue;
        }

        entry->res = std::move(res);
        entry->state = LoadState::GpuLoading;
        m_loadingList.push_back(work.handle);
    }

    m_gpuPending.clear();
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
