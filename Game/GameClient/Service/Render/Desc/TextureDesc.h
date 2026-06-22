#pragma once
#include "Core/Foundation/ResourceID.h"
#include "Core/Utils/Hash.h"

using TextureSlot = uint32_t;

enum class TextureType
{
    Color,
    Linear,
    Count
};

struct TextureDesc
{
    Core::ResourceID resID;
    TextureType type{ TextureType::Color };
    bool generateMips{ true }; // ui : false, 3d : true

    bool operator==(const TextureDesc&) const = default;

    size_t GetHash() const
    {
        return Core::HashOf(
            resID,
            type,
            generateMips);
    }
};