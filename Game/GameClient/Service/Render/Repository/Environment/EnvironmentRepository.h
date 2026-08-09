#pragma once
#include "Core/Utils/Handle/HandlePool.h"
#include "Service/Render/Handle/EnvironmentHandle.h"
#include "../Base/ResourceTypes.h"

struct IEnvironmentProvider;
struct IEnvironmentResource;
struct EnvironmentDesc;
struct CpuPendingEnvironmentRequest;
struct GpuPendingEnvironmentRequest;
struct IAssetAsyncLoader;

struct EnvironmentEntry
{
    size_t key{};
    std::shared_ptr<IEnvironmentResource> envRes;
    LoadState state{ LoadState::Pending };
};

class EnvironmentRepository
{
public:
    ~EnvironmentRepository();
    EnvironmentRepository() = delete;
    EnvironmentRepository(IEnvironmentProvider* envProvider, IAssetAsyncLoader* asyncLoader);

    EnvironmentHandle GetOrCreate(const EnvironmentDesc& desc);
    void Update();
    bool Release(EnvironmentHandle h);
    void ReleaseAll();

    const EnvironmentEntry* Get(EnvironmentHandle h) const noexcept { return m_loadedEnvironments.Find(h); }
    
private:
    void ProcessCpuPending();
    void ProcessGpuPending();
    void ProcessLoading();

    IEnvironmentProvider* m_envProvider{ nullptr };
    IAssetAsyncLoader* m_asyncLoader{ nullptr };

    std::unordered_map<size_t, EnvironmentHandle> m_cache;
    HandlePool<EnvironmentEntry, EnvironmentTag> m_loadedEnvironments;

    std::vector<CpuPendingEnvironmentRequest> m_cpuPending;
    std::vector<GpuPendingEnvironmentRequest> m_gpuPending;

    std::vector<EnvironmentHandle> m_loadingList;
};