#pragma once

enum class BuiltinTextureType
{
    White,  // 일반 컬러/알베도용 (1,1,1,1)
    FlatNormal, // 노멀 맵용 (0.5, 0.5, 1.0)
    DefaultARM, //arm용 (1, 0.5, 0)
    Count
};

struct BuiltinTextureBinding
{
    TextureSlot slot;
    BuiltinTextureType type;
};