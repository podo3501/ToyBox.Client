#pragma once
#include "Core/Utils/Handle/HandlePool.h"
#include "Core/Utils/Hash.h"
#include "../ResourceTypes.h"
#include "Service/Render/Handle/TextureHandle.h"
#include "Service/Render/Desc/TextureDesc.h"
#include "Service/AssetAsync/AssetAsyncTypes.h"

struct ITextureProvider;
struct ITextureResource;
struct TextureAsset;
struct CpuPendingTextureRequest;
struct GpuPendingTextureRequest;

struct TextureEntry
{
    shared_ptr<ITextureResource> texRes;
    LoadState state{ LoadState::Pending };
};

class TextureRepository
{
public:
    ~TextureRepository();
    TextureRepository(ITextureProvider* texProvider, AssetPipelineT* assetPipeline);

    TextureHandle GetOrCreate(const TextureDesc& desc, std::shared_ptr<TextureAsset> asset = nullptr);

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

    ITextureProvider* m_texProvider{ nullptr };
    AssetPipelineT* m_assetPipeline{ nullptr };

    unordered_map<size_t, TextureHandle> m_cache;
    HandlePool<TextureEntry, TextureTag> m_loadedTextures;

    vector<CpuPendingTextureRequest> m_cpuPending;
    vector<GpuPendingTextureRequest> m_gpuPending;
    vector<TextureHandle> m_loadingList;
};

