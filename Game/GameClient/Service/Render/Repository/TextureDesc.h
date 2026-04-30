#pragma once

struct TextureDesc
{
    bool srgb{ true }; // true : color texture, false : data texture (ex. normal map)
    bool generateMips{ true }; // ui : false, 3d : true

    bool operator==(const TextureDesc& rhs) const = default;
};