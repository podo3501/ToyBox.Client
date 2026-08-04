#include "pch.h"
#include "EnvironmentRepository.h"
#include "IEnvironmentProvider.h"
#include "Service/Render/Definition/EnvironmentDesc.h"
#include "Service/IAssetAsyncLoader.h"
#include "Service/AssetAsyncHelper.h"

struct CpuPendingEnvironmentRequest
{
    EnvironmentHandle handle;
    AssetRequestID requestId;
};

struct GpuPendingEnvironmentRequest
{
    EnvironmentHandle handle;
    std::shared_ptr<EnvironmentAsset> asset;
};

EnvironmentRepository::~EnvironmentRepository() = default;
EnvironmentRepository::EnvironmentRepository(IEnvironmentProvider* envProvider, IAssetAsyncLoader* asyncLoader) :
    m_envProvider{ envProvider },
    m_asyncLoader{ asyncLoader }
{}

EnvironmentHandle EnvironmentRepository::GetOrCreate(const EnvironmentDesc& desc)
{
    const size_t key = desc.GetHash();
    auto it = m_cache.find(key);
    if (it != m_cache.end())
        return it->second;

    auto envRes = m_envProvider->CreateResource();
    if (!envRes) return EnvironmentHandle::Invalid();

    EnvironmentEntry entry;
    entry.key = key;
    entry.envRes = std::move(envRes);
    entry.state = LoadState::Pending;

    auto handle = m_loadedEnvironments.Emplace(std::move(entry));
    m_cache[key] = handle;

    auto resType = desc.envmap.GetType();
    switch (resType)
    {
    case Core::ResourceIDType::Path:
    {
        auto reqID = Asset::PushRequest<EnvironmentAsset>(m_asyncLoader, desc.envmap);
        m_cpuPending.push_back({ handle, reqID });
    }
    break;
    case Core::ResourceIDType::Runtime:
    case Core::ResourceIDType::Builtin: //일단 보류
    default:
        return EnvironmentHandle::Invalid();
    }

    return handle;
}

bool EnvironmentRepository::Release(EnvironmentHandle h)
{
    auto entry = m_loadedEnvironments.Find(h);
    if (!entry) return false;

    m_cache.erase(entry->key);
    std::erase(m_loadingList, h);

    m_envProvider->ReleaseResource(std::move(entry->envRes));
    return m_loadedEnvironments.Remove(h);
}

void EnvironmentRepository::ReleaseAll()
{
    m_loadedEnvironments.Visit([this](EnvironmentHandle h, EnvironmentEntry&) {
        Release(h);
        });

    m_cpuPending.clear();
    m_gpuPending.clear();

    m_loadingList.clear();
    m_cache.clear();
    m_loadedEnvironments.Clear();
}

void EnvironmentRepository::Update()
{
    ProcessCpuPending();
    ProcessGpuPending();
    ProcessLoading();
}

void EnvironmentRepository::ProcessCpuPending()
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

        GpuPendingEnvironmentRequest gpuReq;
        gpuReq.handle = req.handle;
        gpuReq.asset = std::static_pointer_cast<EnvironmentAsset>(asset);
        m_gpuPending.push_back(std::move(gpuReq));

        it = m_cpuPending.erase(it);
    }
}

void EnvironmentRepository::ProcessGpuPending()
{
    for (auto& work : m_gpuPending)
    {
        auto entry = m_loadedEnvironments.Find(work.handle);
        if (!entry || !entry->envRes) continue;

        if (!m_envProvider->LoadResource(entry->envRes, work.asset))
        {
            entry->state = LoadState::Failed;
            continue;
        }
        entry->state = LoadState::GpuLoading;

        m_loadingList.push_back(work.handle);
    }

    m_gpuPending.clear();
}

void EnvironmentRepository::ProcessLoading()
{
    for (auto it = m_loadingList.begin(); it != m_loadingList.end(); )
    {
        auto entry = m_loadedEnvironments.Find(*it);
        if (!entry || !entry->envRes)
        {
            it = m_loadingList.erase(it);
            continue;
        }

        if (entry->envRes->IsReady())
        {
            entry->state = LoadState::Ready;
            it = m_loadingList.erase(it);
        }
        else
            ++it;
    }
}