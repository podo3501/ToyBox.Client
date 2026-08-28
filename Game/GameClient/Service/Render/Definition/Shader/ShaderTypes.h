#pragma once
#include <cstdint>

using ShaderID = uint32_t;
inline constexpr ShaderID InvalidShaderID{ 0 };

enum class ShaderType
{
    Graphics,
    Compute
};

enum class RasterPreset
{
    Default,
    NoCull,
    Wireframe,
    WireframeNoCull
};
