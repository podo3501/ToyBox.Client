#pragma once

enum class DefaultTextureType
{
    White,  // 일반 컬러/알베도용 (1,1,1,1)
    FlatNormal, // 노멀 맵용 (0.5, 0.5, 1.0)
    Orange, //arm용 오렌지색(1, 0.5, 0)
    Count
};