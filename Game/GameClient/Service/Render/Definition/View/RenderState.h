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

enum class ShaderType
{
    Graphics,
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

using ShaderID = uint32_t;
inline constexpr ShaderID InvalidShaderID{ 0 };

namespace RegistryShader //이 enum 값은 파일 저장이나 직렬화, 네트워크의 값으로 사용되면 안된다. inspectorImage가 release 일때 빠질수도 있다.
{
    constexpr ShaderID Shadow{ 1 };
    constexpr ShaderID Phong{ 2 };
    constexpr ShaderID PBR{ 3 };
    constexpr ShaderID Grid{ 4 };
    constexpr ShaderID UI{ 5 };
    constexpr ShaderID Skybox{ 6 };
    constexpr ShaderID Composite{ 7 }; // view들을 합성.
    constexpr ShaderID MipGenerator{ 8 }; //compute

    constexpr ShaderID InspectorImage{ 9 }; //테스트용 shader. 이건 release 일때 빠질수도 있다.
}

struct ShaderAsset;
struct ShaderDesc
{
    std::shared_ptr<ShaderAsset> asset;
    std::vector<ShaderStageDesc> stages;
};
using RegistryShaderDesc = std::pair<ShaderID, ShaderDesc>;

struct ShaderVariant
{
    ShaderID shaderID;
    std::vector<ShaderMacroDesc> runtimeMacros;

    bool operator==(const ShaderVariant&) const = default;

    size_t GetHash() const
    {
        size_t h = Core::HashOf(shaderID);
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
        ShaderID shaderID,
        RasterPreset rasterPreset,
        PrimitiveTopologyType topologyType =
        PrimitiveTopologyType::Triangle)
    {
        PipelineState state{};

        state.shaderVariant.shaderID = shaderID;
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