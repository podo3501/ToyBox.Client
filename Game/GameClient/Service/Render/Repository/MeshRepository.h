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
    bool Release(MeshHandle mh);
    void Update();
    const MeshEntry* Get(MeshHandle h) const noexcept { return m_loadedMeshes.Find(h); }

private:
    void ProcessPending();
    void ProcessLoading();

    IMeshSystem* m_meshSystem{ nullptr };
    unordered_map<filesystem::path, MeshHandle> m_cache;
    HandlePool<MeshEntry, MeshTag> m_loadedMeshes;

    vector<MeshPendingRequest> m_pending;
    vector<MeshHandle> m_loadingList;
};
