#pragma once
#include "Core/Utils/Handle/HandlePool.h"
#include "MaterialHandle.h"
#include "TextureHandle.h"

struct ITextureResource;
class TextureRepository;

enum class MaterialState
{
    Pending,
    Ready,
    Failed
};

struct MaterialEntry
{
    ITextureResource* texRes{ nullptr };
    TextureHandle sourceTextureHandle;
    MaterialState state{ MaterialState::Pending };
};

class MaterialRepository
{
public:
    ~MaterialRepository();
    MaterialRepository() = delete;
    explicit MaterialRepository(TextureRepository* texRepository);
    MaterialHandle Create(TextureHandle tex);
    void Update();
    const MaterialEntry* Get(MaterialHandle h) const noexcept { return m_pool.Find(h); }

private:
    TextureRepository* m_texRepository{ nullptr };
    HandlePool<MaterialEntry, MaterialTag> m_pool;
    vector<MaterialHandle> m_loadingList;
};