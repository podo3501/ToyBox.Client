#pragma once

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
    FillMode fillMode = FillMode::Solid;
    CullMode cullMode = CullMode::Back;
};