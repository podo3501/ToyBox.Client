#pragma once
#include "Core/Utils/Handle/HandlePool.h"
#include "Core/Utils/Hash.h"
#include "MaterialHandle.h"
#include "ResourceTypes.h"
#include "TextureDesc.h"
#include "MaterialDesc.h"

struct IMaterialSystem;
struct IMaterialResource;
struct TextureAsset;
struct CpuPendingMaterialRequest;
struct GpuPendingMaterialRequest;

struct MaterialKey
{
    ResourceKey resourceKey;
    MaterialDesc matDesc;

    bool operator==(const MaterialKey& rhs) const = default;
};

struct MaterialKeyHash
{
    size_t operator()(const MaterialKey& k) const
    {
        return Core::HashOf(
            k.resourceKey.GetHash(),
            k.matDesc.GetHash());
    }
};

struct MaterialEntry
{
    MaterialKey key;
    std::shared_ptr<IMaterialResource> matRes;
    LoadState state{ LoadState::Pending };
};

class MaterialRepository
{
public:
    ~MaterialRepository();
    MaterialRepository() = delete;
    MaterialRepository(IMaterialSystem* matSystem);

    MaterialHandle GetOrCreate(
        std::filesystem::path path,
        const MaterialDesc& desc,
        function<shared_ptr<TextureAsset>(const filesystem::path&)> loader);

    MaterialHandle GetOrCreate(
        const std::string& runtimeKey, 
        shared_ptr<TextureAsset> albedoAsset, 
        const MaterialDesc& desc);

    bool Release(MaterialHandle h);
    void ReleaseAll();
    void Update();
    const MaterialEntry* Get(MaterialHandle h) const noexcept { return m_loadedMaterials.Find(h); }

private:
    void ProcessCpuPending();
    void ProcessGpuPending();
    void ProcessLoading();

    IMaterialSystem* m_matSystem{ nullptr };
    std::unordered_map<MaterialKey, MaterialHandle, MaterialKeyHash> m_cache;
    HandlePool<MaterialEntry, MaterialTag> m_loadedMaterials;

    std::vector<CpuPendingMaterialRequest> m_cpuPending;
    std::vector<GpuPendingMaterialRequest> m_gpuPending;
    std::vector<MaterialHandle> m_loadingList;
};