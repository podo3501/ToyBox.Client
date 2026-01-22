#pragma once

enum class SrvOffset : size_t
{
    Imgui = 50,
    RenderTexture = 51,
    Count = 100,
};

constexpr int MAX_DESC = 100;