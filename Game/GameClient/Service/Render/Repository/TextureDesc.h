#pragma once
#include <filesystem>

struct TextureDesc
{
    bool srgb{ true }; // true : color texture, false : data texture (ex. normal map)
    bool generateMips{ true }; // ui : false, 3d : true

    bool operator==(const TextureDesc& rhs) const = default;
};

struct TextureLoadDesc
{
    std::filesystem::path path;
    TextureDesc texDesc;

    bool operator==(const TextureLoadDesc&) const = default;
    TextureDesc ToCreateDesc() const { return texDesc; }
};