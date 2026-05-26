#pragma once
#include "Core/Utils/Handle/HandlePool.h"
#include "Core/Utils/Hash.h"
#include "../ResourceTypes.h"
#include "Service/Render/Handle/TextureHandle.h"
#include "Service/Render/Desc/TextureBinding.h"

struct ITextureSystem;
struct ITextureResource;
struct TextureAsset;
struct CpuPendingTextureRequest;
struct GpuPendingTextureRequest;

struct TextureKey
{
    ResourceKey resourceKey;
    TextureDesc desc;

    bool operator==(const TextureKey& rhs) const = default;
};

struct TextureKeyHash
{
    size_t operator()(const TextureKey& k) const
    {
        return Core::HashOf(
            k.resourceKey.GetHash(),
            k.desc.GetHash());
    }
};

struct TextureEntry
{
    TextureKey key;
    shared_ptr<ITextureResource> texRes;
    LoadState state{ LoadState::Pending };
};

class TextureRepository
{
public:
    ~TextureRepository();
    explicit TextureRepository(ITextureSystem* texSystem);

    TextureHandle GetOrCreate(
        std::filesystem::path path,
        const TextureDesc& desc,
        function<shared_ptr<TextureAsset>(const filesystem::path&)> loader);

    TextureHandle GetOrCreate(
        const std::string& runtimeKey,
        std::shared_ptr<TextureAsset> asset,
        const TextureDesc& desc);

    bool Release(TextureHandle h);
    void ReleaseAll();
    void Update();
    const TextureEntry* Get(TextureHandle h) const noexcept { return m_loadedTextures.Find(h); }

private:
    void ProcessCpuPending();
    void ProcessGpuPending();
    void ProcessLoading();

    ITextureSystem* m_texSystem{ nullptr };
    unordered_map<TextureKey, TextureHandle, TextureKeyHash> m_cache;
    HandlePool<TextureEntry, TextureTag> m_loadedTextures;

    vector<CpuPendingTextureRequest> m_cpuPending;
    vector<GpuPendingTextureRequest> m_gpuPending;
    vector<TextureHandle> m_loadingList;
};

