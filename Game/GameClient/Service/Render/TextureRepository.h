#pragma once
#include "Core/Utils/Handle/HandlePool.h"
#include "TextureHandle.h"

struct IRenderBackend;
struct ITextureResource;
struct TextureAsset;
struct PendingRequest;
enum class TextureState;

enum class TextureState
{
    Pending,
    Loading,
    Ready,
    Failed
};

struct TextureEntry
{
    shared_ptr<ITextureResource> texRes;
    TextureState state{ TextureState::Pending };
    bool inLoadingList{ false };
};

class TextureRepository
{
public:
    ~TextureRepository();
    explicit TextureRepository(IRenderBackend* backend);

    TextureHandle GetOrCreate(const filesystem::path& path,
        function<shared_ptr<TextureAsset>(const filesystem::path&)> loader);
    bool Release(TextureHandle handle);
    void Update();
    const TextureEntry* Get(TextureHandle handle) const noexcept;

private:
    void ProcessPending();
    void ProcessLoading();

private:
    IRenderBackend* m_backend{ nullptr };
    unordered_map<filesystem::path, weak_ptr<ITextureResource>> m_cache;
    HandlePool<TextureEntry, TextureTag> m_loadedTextures;

    vector<PendingRequest> m_pending;
    vector<TextureHandle> m_loadingList;
};

