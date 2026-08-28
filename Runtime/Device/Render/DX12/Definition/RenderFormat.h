#pragma once
#include <dxgiformat.h>

namespace RenderFormat
{
    constexpr DXGI_FORMAT BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    constexpr DXGI_FORMAT BackBufferSRGBView = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

    constexpr DXGI_FORMAT DepthFormat = DXGI_FORMAT_D32_FLOAT;
    constexpr DXGI_FORMAT ShadowMapFormat = DXGI_FORMAT_D32_FLOAT;
}