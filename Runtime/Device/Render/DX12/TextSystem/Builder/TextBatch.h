#pragma once
#include <unordered_map>
#include <memory>
#include <vector>
#include "../Atlas/FontSetting.h"
#include "Core/Utils/Hash.h"

struct UIVertex;
struct IMaterialResource;
struct PageMeshBuffer
{
    std::vector<UIVertex> vertices;
    std::vector<uint32_t> indices;
    uint32_t vertexOffset = 0;

    std::shared_ptr<IMaterialResource> material{ nullptr };
};

struct TextBatchKey
{
    FontBucketID bucket{ InvalidFontBucket };
    uint16_t pageIndex{ 0 };

    bool operator==(const TextBatchKey&) const = default;
};

struct TextBatchKeyHash
{
    size_t operator()(const TextBatchKey& key) const
    {
        return Core::HashOf(
            key.bucket,
            key.pageIndex);
    }
};

using TextBatchBufferMap = std::unordered_map<TextBatchKey, PageMeshBuffer, TextBatchKeyHash>;