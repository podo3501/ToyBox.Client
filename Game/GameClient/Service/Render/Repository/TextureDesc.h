#pragma once
#include <filesystem>
#include "Core/Utils/Hash.h"

struct TextureAsset;

struct TextureDesc
{
    bool srgb{ true }; // true : color texture, false : data texture (ex. normal map)
    bool generateMips{ true }; // ui : false, 3d : true

    bool operator==(const TextureDesc& other) const = default;

    size_t GetHash() const
    {
        return Core::HashOf(srgb, generateMips);
    }
};