#pragma once
#include "Core/Utils/Handle/HandlePool.h"
#include "Service/Render/Handle/BrushHandle.h"
#include "../ResourceTypes.h"

struct IBrushProvider;
struct IBrushResource;
struct BrushDesc;
struct CpuPendingBrushRequest;
struct GpuPendingBrushRequest;
struct IAssetAsyncLoader;

struct BrushEntry
{
    size_t key{};
    std::shared_ptr<IBrushResource> brushRes;
    LoadState state{ LoadState::Pending };
};

class BrushRepository
{
public:
    ~BrushRepository();
    BrushRepository() = delete;
    BrushRepository(IBrushProvider* brushProvider, IAssetAsyncLoader* asyncLoader);

    BrushHandle GetOrCreate(const BrushDesc& desc);
    void Update();
    bool Release(BrushHandle h);
    void ReleaseAll();

    const BrushEntry* Get(BrushHandle h) const noexcept { return m_loadedBrushes.Find(h); }

private:
    void ProcessCpuPending();
    void ProcessGpuPending();
    void ProcessLoading();

    IBrushProvider* m_brushProvider{ nullptr };
    IAssetAsyncLoader* m_asyncLoader{ nullptr };

    std::unordered_map<size_t, BrushHandle> m_cache;
    HandlePool<BrushEntry, BrushTag> m_loadedBrushes;

    std::vector<CpuPendingBrushRequest> m_cpuPending;
    std::vector<GpuPendingBrushRequest> m_gpuPending;

    std::vector<BrushHandle> m_loadingList;
};
