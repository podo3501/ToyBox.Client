#pragma once
#include <filesystem>
#include "TextureDesc.h"

struct TextureAsset;

struct MaterialSurface
{
    float roughness{ 0.5f };
    float metallic{ 0.0f };

    bool operator==(const MaterialSurface&) const = default;
};

struct MaterialDesc
{
    std::shared_ptr<TextureAsset> albedoAsset{ nullptr };
    TextureDesc albedoDesc;
    MaterialSurface surface;
};

struct MaterialLoadDesc
{
    TextureLoadDesc albedoLoadDesc;
    MaterialSurface surface;

    MaterialDesc ToCreateDesc() const
    {
        MaterialDesc desc;
        desc.albedoDesc = albedoLoadDesc.ToCreateDesc();
        desc.surface = surface;

        return desc;
    }
};