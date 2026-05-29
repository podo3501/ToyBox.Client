#pragma once
#include "Core/Foundation/ResourceID.h"
#include "Core/Utils/Hash.h"

using TextureSlot = uint32_t;

struct TextureDesc
{
    Core::ResourceID resID;
    bool srgb{ true }; // true : color texture, false : data texture (ex. normal map)
    bool generateMips{ true }; // ui : false, 3d : true

    bool operator==(const TextureDesc&) const = default;

    size_t GetHash() const
    {
        return Core::HashOf(
            resID,
            srgb, 
            generateMips);
    }
};