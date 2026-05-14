#pragma once
#include "Core/Utils/Handle/HandlePool.h"
#include "MaterialHandle.h"
#include "TextureHandle.h"
#include "ResourceTypes.h"
#include "TextureDesc.h"

//struct ITextureResource;

//struct MaterialEntry
//{
//    ITextureResource* texRes{ nullptr };
//    TextureHandle sourceTextureHandle;
//    LoadState state{ LoadState::Pending };
//};

struct IMaterialSystem;
struct IMaterialResource;
struct ITextureSystem;
struct ITextureResource;
struct TextureAsset;
struct CpuPendingMaterialRequest;
struct GpuPendingMaterialRequest;

struct MaterialKey
{
    std::filesystem::path path;
    TextureDesc desc;

    bool operator==(const MaterialKey& rhs) const = default;
};

struct MaterialKeyHash
{
    size_t operator()(const MaterialKey& k) const
    {
        size_t h1 = std::hash<std::string>()(k.path.string());

        size_t h2 = 0;
        h2 ^= std::hash<bool>()(k.desc.srgb) << 1;
        h2 ^= std::hash<bool>()(k.desc.generateMips) << 2;

        return h1 ^ (h2 << 1);
    }
};

struct MaterialEntry
{
    MaterialKey key;
    std::shared_ptr<IMaterialResource> matRes;
    std::shared_ptr<ITextureResource> texRes;
    LoadState state{ LoadState::Pending };
};

class MaterialRepository
{
public:
    ~MaterialRepository();
    MaterialRepository() = delete;
    MaterialRepository(IMaterialSystem* matSystem, ITextureSystem* texSystem);

    MaterialHandle GetOrCreate(const std::filesystem::path& path, const TextureDesc& desc,
        function<shared_ptr<TextureAsset>(const std::filesystem::path&)> loader);
    bool Release(MaterialHandle h);
    void ReleaseAll();
    void Update();
    const MaterialEntry* Get(MaterialHandle h) const noexcept { return m_loadedMaterials.Find(h); }

private:
    void ProcessCpuPending();
    void ProcessGpuPending();
    void ProcessLoading();

    IMaterialSystem* m_matSystem{ nullptr };
    ITextureSystem* m_texSystem{ nullptr };
    std::unordered_map<MaterialKey, MaterialHandle, MaterialKeyHash> m_cache;
    HandlePool<MaterialEntry, MaterialTag> m_loadedMaterials;

    std::vector<CpuPendingMaterialRequest> m_cpuPending;
    std::vector<GpuPendingMaterialRequest> m_gpuPending;
    std::vector<MaterialHandle> m_loadingList;
};