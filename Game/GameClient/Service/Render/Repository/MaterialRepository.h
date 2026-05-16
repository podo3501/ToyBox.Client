#pragma once
#include "Core/Utils/Handle/HandlePool.h"
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
    TextureLoadDesc albedoLoadDesc;
    MaterialSurface surface;

    bool operator==(const MaterialKey& rhs) const = default;
};

struct MaterialKeyHash
{
    size_t operator()(const MaterialKey& k) const
    {
        size_t h = 0;

        h ^= std::hash<std::string>()(k.albedoLoadDesc.path.string());
        h ^= std::hash<bool>()(k.albedoLoadDesc.texDesc.srgb) << 1;
        h ^= std::hash<bool>()(k.albedoLoadDesc.texDesc.generateMips) << 2;
        h ^= std::hash<float>()(k.surface.roughness) << 3;
        h ^= std::hash<float>()(k.surface.metallic) << 4;

        return h;
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

    MaterialHandle GetOrCreate(const MaterialLoadDesc& loadDesc,
        function<shared_ptr<TextureAsset>(const filesystem::path&)> loader);
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