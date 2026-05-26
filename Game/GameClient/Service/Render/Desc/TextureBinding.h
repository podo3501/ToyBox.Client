#pragma once
#include "Core/Foundation/ResourceID.h"
#include "Core/Utils/Hash.h"

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

enum class TextureSlot
{
    Albedo,
    Normal,
    Count
};

struct TextureBinding
{
    Core::ResourceID resID;
    TextureDesc desc{};

    bool operator==(const TextureBinding&) const = default;

    size_t GetHash() const
    {
        return Core::HashOf(
            resID,
            desc.GetHash());
    }
};