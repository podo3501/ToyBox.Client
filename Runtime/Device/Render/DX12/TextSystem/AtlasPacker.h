#pragma once
#include "Core/Foundation/Geometry2D.h"
#include "stb_rect_pack.h"

class AtlasPacker
{
public:
    ~AtlasPacker();
    AtlasPacker();

    AtlasPacker(const AtlasPacker&) = delete;
    AtlasPacker& operator=(const AtlasPacker&) = delete;

    AtlasPacker(AtlasPacker&&) noexcept = default;
    AtlasPacker& operator=(AtlasPacker&&) noexcept = default;

    void Initialize(const Size& textureSize);

    Point AllocateRect(const Size& size);
    void Reset();

private:
    Size m_textureSize;
    std::unique_ptr<stbrp_context> m_context;
    std::vector<stbrp_node> m_nodes;
};
