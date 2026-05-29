#pragma once
#include "Core/Utils/Handle/HandlePool.h"
#include "Core/Utils/Hash.h"
#include "../ResourceTypes.h"
#include "Service/Render/Handle/MaterialHandle.h"
#include "Service/Render/Desc/TextureDesc.h"
#include "Service/Render/Desc/MeshMaterialDesc.h"
#include "Service/Render/Desc/UIMaterialDesc.h"
#include "Service/AssetAsync/AssetAsyncTypes.h"

struct IMaterialSystem;
struct IMaterialResource;
struct Asset;
struct CpuPendingMaterialRequest;
struct GpuPendingMaterialRequest;

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
    MaterialRepository(IMaterialSystem* matSystem, AssetPipelineT* assetPipeline);

    MaterialHandle GetOrCreate(const MaterialDesc& desc);

    //MaterialHandle GetOrCreate(
    //    std::filesystem::path path,
    //    std::unique_ptr<MaterialDesc> desc,
    //    function<shared_ptr<TextureAsset>(const filesystem::path&)> loader);

    //MaterialHandle GetOrCreate(
    //    const std::string& runtimeKey, 
    //    shared_ptr<TextureAsset> albedoAsset, 
    //    std::unique_ptr<MaterialDesc> desc);

    void Update();
    bool Release(MaterialHandle h);
    void ReleaseAll();
    const MaterialEntry* Get(MaterialHandle h) const noexcept { return m_loadedMaterials.Find(h); }

private:
    void ProcessCpuPending();
    void ProcessGpuPending();
    void ProcessLoading();

    IMaterialSystem* m_matSystem{ nullptr };
    AssetPipelineT* m_assetPipeline{ nullptr };

    std::unordered_map<size_t, MaterialHandle> m_cache;
    HandlePool<MaterialEntry, MaterialTag> m_loadedMaterials;

    std::vector<CpuPendingMaterialRequest> m_cpuPending;
    std::vector<GpuPendingMaterialRequest> m_gpuPending;
    std::vector<MaterialHandle> m_loadingList;
};