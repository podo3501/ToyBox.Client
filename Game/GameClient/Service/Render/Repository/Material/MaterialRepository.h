#pragma once
#include "Core/Utils/Handle/HandlePool.h"
#include "Core/Utils/Hash.h"
#include "../ResourceTypes.h"
#include "Service/Render/Handle/MaterialHandle.h"
#include "Service/Render/Desc/TextureDesc.h"
#include "Service/AssetAsync/AssetAsyncTypes.h"

struct MaterialDesc;
struct IMaterialProvider;
struct IMaterialResource;
struct AssetData;
struct CpuPendingMaterialRequest;
struct PendingMaterialTextures;
struct GpuPendingMaterialRequest;
class AssetPipeline;

struct MaterialEntry
{
    size_t key;
    std::shared_ptr<IMaterialResource> matRes;
    LoadState state{ LoadState::Pending };
};

class MaterialRepository
{
public:
    ~MaterialRepository();
    MaterialRepository() = delete;
    MaterialRepository(IMaterialProvider* matProvider, AssetPipeline* assetPipeline);
    MaterialHandle GetOrCreate(const MaterialDesc& desc);
    void Update();
    bool Release(MaterialHandle h);
    void ReleaseAll();
    const MaterialEntry* Get(MaterialHandle h) const noexcept { return m_loadedMaterials.Find(h); }

private:
    void ProcessCpuPending();
    void ProcessGpuPending();
    void ProcessLoading();

    IMaterialProvider* m_matProvider{ nullptr };
    AssetPipeline* m_assetPipeline{ nullptr };

    std::unordered_map<size_t, MaterialHandle> m_cache;
    HandlePool<MaterialEntry, MaterialTag> m_loadedMaterials;

    std::vector<CpuPendingMaterialRequest> m_cpuPending;
    std::unordered_map<MaterialHandle, PendingMaterialTextures> m_pendingTextures;
    std::vector<GpuPendingMaterialRequest> m_gpuPending;
    std::vector<MaterialHandle> m_loadingList;
};