#pragma once
#include "Core/Utils/Handle/HandlePool.h"
#include "TextureHandle.h"
#include "TextureDesc.h"

struct ITextureSystem;
struct ITextureResource;
struct TextureAsset;
struct PendingRequest;

struct TextureKey
{
    filesystem::path path;
    TextureDesc desc;

    bool operator==(const TextureKey& rhs) const = default;
};

struct TextureKeyHash
{
    size_t operator()(const TextureKey& k) const
    {
        size_t h1 = std::hash<std::string>()(k.path.string());

        size_t h2 = 0;
        h2 ^= std::hash<bool>()(k.desc.srgb) << 1;
        h2 ^= std::hash<bool>()(k.desc.generateMips) << 2;

        return h1 ^ (h2 << 1);
    }
};

enum class TextureState
{
    Pending,
    Loading,
    Ready,
    Failed
};

struct TextureEntry
{
    TextureKey key;
    shared_ptr<ITextureResource> texRes;
    TextureState state{ TextureState::Pending };
};

class TextureRepository
{
public:
    ~TextureRepository();
    explicit TextureRepository(ITextureSystem* texSystem);

    TextureHandle GetOrCreate(const filesystem::path& path, const TextureDesc& desc,
        function<shared_ptr<TextureAsset>(const filesystem::path&)> loader);
    bool Release(TextureHandle handle);
    void Update();
    const TextureEntry* Get(TextureHandle handle) const noexcept { return m_loadedTextures.Find(handle); }

private:
    void ProcessPending();
    void ProcessLoading();

private:
    ITextureSystem* m_texSystem{ nullptr };
    unordered_map<TextureKey, TextureHandle, TextureKeyHash> m_cache;
    HandlePool<TextureEntry, TextureTag> m_loadedTextures;

    vector<PendingRequest> m_pending;
    vector<TextureHandle> m_loadingList;
};

