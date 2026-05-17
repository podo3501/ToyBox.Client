#pragma once
#include "Core/Utils/Handle/HandlePool.h"
#include "MeshHandle.h"
#include "ResourceTypes.h"

struct IMeshSystem;
struct IMeshResource;
struct MeshAsset;
struct MeshPendingRequest;

struct MeshEntry
{
    filesystem::path path;
    shared_ptr<IMeshResource> meshRes;
    LoadState state{ LoadState::Pending };
};

class MeshRepository
{
public:
    ~MeshRepository();
    explicit MeshRepository(IMeshSystem* meshSystem);
    
    MeshHandle GetOrCreate(const filesystem::path& path, function<shared_ptr <MeshAsset > (const filesystem::path&) > loader);
    MeshHandle GetOrCreate(const std::string& runtimeKey, std::shared_ptr<MeshAsset> asset);
    bool Release(MeshHandle mh);
    void ReleaseAll();
    void Update();
    const MeshEntry* Get(MeshHandle h) const noexcept { return m_loadedMeshes.Find(h); }

private:
    void ProcessPending();
    void ProcessLoading();

    IMeshSystem* m_meshSystem{ nullptr };
    std::unordered_map<ResourceKey, MeshHandle, ResourceKeyHash> m_cache;
    HandlePool<MeshEntry, MeshTag> m_loadedMeshes;

    vector<MeshPendingRequest> m_pending;
    vector<MeshHandle> m_loadingList;
};
