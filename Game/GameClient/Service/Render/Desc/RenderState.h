#pragma once
#include "Core/Utils/Hash.h"

enum class FillMode
{
    Solid,
    Wireframe
};

enum class CullMode
{
    None,
    Front,
    Back
};

struct RasterState
{
    FillMode fillMode{ FillMode::Solid };
    CullMode cullMode{ CullMode::Back };

    bool operator==(const RasterState&) const = default;
};

enum class RasterPreset
{
    Default,
    NoCull,
    Wireframe,
    WireframeNoCull
};

class RasterLibrary
{
public:
    static RasterState Get(RasterPreset preset)
    {
        switch (preset)
        {
        case RasterPreset::Default:
            return {
                FillMode::Solid,
                CullMode::Back
            };

        case RasterPreset::NoCull:
            return {
                FillMode::Solid,
                CullMode::None
            };

        case RasterPreset::Wireframe:
            return {
                FillMode::Wireframe,
                CullMode::Back
            };

        case RasterPreset::WireframeNoCull:
            return {
                FillMode::Wireframe,
                CullMode::None
            };
        }

        return {};
    }
};

enum class ShaderStage
{
    Vertex,
    Pixel,
    Compute
};

struct ShaderMacroDesc
{
    std::string name;
    std::string value{ "1" };

    bool operator==(const ShaderMacroDesc&) const = default;

    size_t GetHash() const
    {
        return Core::HashOf(name, value);
    }
};

struct ShaderStageDesc
{
    ShaderStage stage;
    std::string entry;
    std::string target;
};

using ShaderKey = uint32_t;
inline constexpr ShaderKey InvalidShaderKey{ 0 };

namespace BuiltinShader
{
    constexpr ShaderKey Shadow{ 1 };
    constexpr ShaderKey Phong{ 2 };
    constexpr ShaderKey PBR{ 3 };
    constexpr ShaderKey Grid{ 4 };
    constexpr ShaderKey UI{ 5 };
    constexpr ShaderKey MipGenerator{ 6 }; //compute
}

struct ShaderAsset;
struct ShaderDesc
{
    std::shared_ptr<ShaderAsset> asset;
    std::vector<ShaderStageDesc> stages;
};

using BuiltinShaderDesc = std::pair<ShaderKey, ShaderDesc>;

struct ShaderVariant
{
    ShaderKey shaderKey;
    std::vector<ShaderMacroDesc> runtimeMacros;

    bool operator==(const ShaderVariant&) const = default;

    size_t GetHash() const
    {
        size_t h = Core::HashOf(shaderKey);
        for (const auto& macro : runtimeMacros)
            Core::HashCombine(h, macro.GetHash());
        return h;
    }
};

struct ShaderVariantHasher
{
    size_t operator()(const ShaderVariant& variant) const
    {
        return variant.GetHash();
    }
};

enum class PrimitiveTopologyType
{
    Triangle,
    Line
};

struct PipelineState
{
    ShaderVariant shaderVariant{};
    RasterState rasterState{};
    PrimitiveTopologyType topologyType{ PrimitiveTopologyType::Triangle };

    bool operator==(const PipelineState&) const = default;

    size_t GetHash() const
    {
        return Core::HashOf(
            shaderVariant.GetHash(),
            rasterState.fillMode,
            rasterState.cullMode,
            topologyType);
    }
};

struct PipelineStateHasher
{
    size_t operator()(const PipelineState& state) const
    {
        return state.GetHash();
    }
};

class PipelineLibrary
{
public:
    static PipelineState Get(
        ShaderKey shaderKey,
        RasterPreset rasterPreset,
        PrimitiveTopologyType topologyType =
        PrimitiveTopologyType::Triangle)
    {
        PipelineState state{};

        state.shaderVariant.shaderKey = shaderKey;
        state.rasterState = RasterLibrary::Get(rasterPreset);
        state.topologyType = topologyType;

        return state;
    }

    static PipelineState Get(
        ShaderVariant shaderVariant,
        RasterPreset rasterPreset,
        PrimitiveTopologyType topologyType =
        PrimitiveTopologyType::Triangle)
    {
        PipelineState state{};

        state.shaderVariant = shaderVariant;      
        state.rasterState = RasterLibrary::Get(rasterPreset);
        state.topologyType = topologyType;

        return state;
    }
};