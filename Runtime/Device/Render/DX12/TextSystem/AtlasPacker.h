#pragma once
#include "Core/Foundation/Geometry2D.h"

struct stbrp_context;
struct stbrp_node;

class AtlasPacker
{
public:
    ~AtlasPacker();
    AtlasPacker(const Size& textureSize);

    std::pair<int, int> AllocateRect(int width, int height);
    void Reset();

private:
    void Initialize(const Size& textureSize);

    Size m_textureSize;
    stbrp_context* m_context{ nullptr };
    std::vector<stbrp_node> m_nodes;
};
