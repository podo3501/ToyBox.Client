#pragma once
#include "d3d12.h"

inline void SetPremultipliedAlphaBlend(D3D12_RENDER_TARGET_BLEND_DESC& blend)
{
    blend.BlendEnable = TRUE;
    blend.LogicOpEnable = FALSE;

    // PMA 컬러 블렌딩 공식: (셰이더가 낸 rgb, 이미 alpha가 곱해진 상태) + (배경 * (1 - alpha))
    // Straight일 때는 SRC_ALPHA * rgb를 GPU가 곱해줬는데,
    // PMA는 셰이더가 이미 rgb*alpha를 계산해서 내놓으므로 GPU가 또 곱하면 안 됨 -> ONE
    blend.SrcBlend = D3D12_BLEND_ONE;
    blend.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
    blend.BlendOp = D3D12_BLEND_OP_ADD;

    // 알파 채널 자체의 블렌딩 공식
    blend.SrcBlendAlpha = D3D12_BLEND_ONE;
    //blend.DestBlendAlpha = D3D12_BLEND_ZERO;
    blend.DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
    blend.BlendOpAlpha = D3D12_BLEND_OP_ADD;

    blend.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
}