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

enum class PrimitiveTopologyType
{
    Triangle,
    Line
};

struct PipelineState
{
    RasterState rasterState{};
    PrimitiveTopologyType topologyType{ PrimitiveTopologyType::Triangle };

    bool operator==(const PipelineState&) const = default;

    size_t GetHash() const
    {
        return Core::HashOf(
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
        RasterPreset rasterPreset,
        PrimitiveTopologyType topologyType =
        PrimitiveTopologyType::Triangle)
    {
        PipelineState state{};

        state.rasterState = RasterLibrary::Get(rasterPreset);
        state.topologyType = topologyType;

        return state;
    }
};