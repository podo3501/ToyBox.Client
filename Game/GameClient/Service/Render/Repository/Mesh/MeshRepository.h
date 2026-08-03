#pragma once
#include "Core/Utils/Handle/HandlePool.h"
#include "Core/Foundation/ResourceID.h"
#include "Service/Render/Handle/MeshHandle.h"
#include "../ResourceTypes.h"

struct IMeshProvider;
struct IMeshResource;
struct MeshAsset;
struct MeshDesc;
struct CpuPendingMeshRequest;
struct GpuPendingMeshRequest;
struct IAssetAsyncLoader;

struct MeshEntry
{
    Core::ResourceID resID;
    std::shared_ptr<IMeshResource> meshRes;
    LoadState state{ LoadState::Pending };
};

class MeshRepository
{
public:
    ~MeshRepository();
    MeshRepository(IMeshProvider* meshProvider, IAssetAsyncLoader* asyncLoader);
    
    MeshHandle GetOrCreate(const MeshDesc& desc, std::shared_ptr<MeshAsset> asset = nullptr);
    bool Release(MeshHandle mh);
    void ReleaseAll();
    void Update();

    const MeshEntry* Get(MeshHandle h) const noexcept { return m_loadedMeshes.Find(h); }

private:
    void ProcessCpuPending();
    void ProcessGpuPending();
    void ProcessLoading();

    IMeshProvider* m_meshProvider{ nullptr };
    IAssetAsyncLoader* m_asyncLoader{ nullptr };

    std::unordered_map<Core::ResourceID, MeshHandle> m_cache;
    HandlePool<MeshEntry, MeshTag> m_loadedMeshes;

    std::vector<CpuPendingMeshRequest> m_cpuPending;
    std::vector<GpuPendingMeshRequest> m_gpuPending;

    vector<MeshHandle> m_loadingList;
};
