#pragma once
#include "Resource/Resource.h"
#include "Core/Foundation/Geometry2D.h"
#include "GameClient/Asset/TextureTypes.h"

struct TextureCubeDesc
{
    ColorSpace colorSpace{ ColorSpace::Linear }; // 큐브맵은 항상 Linear
};

class TextureCubeResource
{
public:
    ~TextureCubeResource();
    TextureCubeResource();

    bool IsReady() const noexcept { return m_ready; }
    void MarkReady() { m_ready = true; }

    void SetDesc(const TextureCubeDesc& desc) { m_desc = desc; }
    const TextureCubeDesc& GetDesc() const noexcept { return m_desc; }
    const Resource& Get() { return m_texture; }
    void Set(Resource resource) { m_texture = std::move(resource); }
    void SetSize(const Size& size) { m_size = size; }
    const Size& GetSize() const noexcept { return m_size; }
    void SetHeapIndex(UINT index) noexcept { m_heapIndex = index; }
    UINT GetHeapIndex() const noexcept { return m_heapIndex; }

private:
    TextureCubeDesc m_desc;
    Resource m_texture;

    Size m_size;
    UINT m_heapIndex{ UINT_MAX };
    bool m_ready{ false };
};