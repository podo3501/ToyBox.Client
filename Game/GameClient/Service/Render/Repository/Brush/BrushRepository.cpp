#include "pch.h"
#include "BrushRepository.h"
#include "IBrushProvider.h"
#include "Service/Render/Definition/BrushDesc.h"
#include "Service/IAssetAsyncLoader.h"
#include "Service/AssetAsyncHelper.h"

struct CpuPendingBrushRequest
{
    BrushHandle handle;
    AssetRequestID requestId;
};

struct GpuPendingBrushRequest
{
    BrushHandle handle;
    std::shared_ptr<TextureAsset> asset;
};

BrushRepository::~BrushRepository() = default;
BrushRepository::BrushRepository(IBrushProvider* brushProvider, IAssetAsyncLoader* asyncLoader) :
    m_brushProvider{ brushProvider },
    m_asyncLoader{ asyncLoader }
{}

BrushHandle BrushRepository::GetOrCreate(const BrushDesc& desc)
{
    const size_t key = desc.GetHash();
    auto it = m_cache.find(key);
    if (it != m_cache.end())
        return it->second;

    auto brushRes = m_brushProvider->CreateResource();
    if (!brushRes) return BrushHandle::Invalid();

    BrushEntry entry;
    entry.key = key;
    entry.brushRes = std::move(brushRes);
    entry.state = LoadState::Pending;

    auto handle = m_loadedBrushes.Emplace(std::move(entry));
    m_cache[key] = handle;

    auto resType = desc.texture.GetType();
    switch (resType)
    {
    case Core::ResourceIDType::Path:
    {
        auto reqID = Asset::PushRequest<TextureAsset>(m_asyncLoader, desc.texture);
        m_cpuPending.push_back({ handle, reqID });
    }
    break;
    case Core::ResourceIDType::Runtime:
    case Core::ResourceIDType::Builtin:
    default:
        return BrushHandle::Invalid();
    }

    return handle;
}

bool BrushRepository::Release(BrushHandle h)
{
    auto entry = m_loadedBrushes.Find(h);
    if (!entry) return false;

    m_cache.erase(entry->key);
    std::erase(m_loadingList, h);

    m_brushProvider->ReleaseResource(std::move(entry->brushRes));
    return m_loadedBrushes.Remove(h);
}

void BrushRepository::ReleaseAll()
{
    m_loadedBrushes.Visit([this](BrushHandle h, BrushEntry&) {
        Release(h);
        });

    m_cpuPending.clear();
    m_gpuPending.clear();

    m_loadingList.clear();
    m_cache.clear();
    m_loadedBrushes.Clear();
}

void BrushRepository::Update()
{
    ProcessCpuPending();
    ProcessGpuPending();
    ProcessLoading();
}

void BrushRepository::ProcessCpuPending()
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

        GpuPendingBrushRequest gpuReq;
        gpuReq.handle = req.handle;
        gpuReq.asset = std::static_pointer_cast<TextureAsset>(asset);
        m_gpuPending.push_back(std::move(gpuReq));

        it = m_cpuPending.erase(it);
    }
}

void BrushRepository::ProcessGpuPending()
{
    for (auto& work : m_gpuPending)
    {
        auto entry = m_loadedBrushes.Find(work.handle);
        if (!entry || !entry->brushRes) continue;

        if (!m_brushProvider->LoadResource(entry->brushRes, work.asset))
        {
            entry->state = LoadState::Failed;
            continue;
        }
        entry->state = LoadState::GpuLoading;

        m_loadingList.push_back(work.handle);
    }

    m_gpuPending.clear();
}

void BrushRepository::ProcessLoading()
{
    for (auto it = m_loadingList.begin(); it != m_loadingList.end(); )
    {
        auto entry = m_loadedBrushes.Find(*it);
        if (!entry || !entry->brushRes)
        {
            it = m_loadingList.erase(it);
            continue;
        }

        if (entry->brushRes->IsReady())
        {
            entry->state = LoadState::Ready;
            it = m_loadingList.erase(it);
        }
        else
            ++it;
    }
}