#pragma once

struct UIVertex;
class BrushResource;

struct UIBatchBuffer
{
    std::vector<UIVertex> vertices;
    std::vector<uint32_t> indices;
    std::shared_ptr<BrushResource> brush{ nullptr };
};
