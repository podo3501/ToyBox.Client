#pragma once
#include "d3dx12.h"

namespace CommandUtils
{
    // 상태
    void Transition(
        ID3D12GraphicsCommandList* cmd,
        ID3D12Resource* resource,
        D3D12_RESOURCE_STATES before,
        D3D12_RESOURCE_STATES after) noexcept;

    // 프레임 기본
    void SetViewport(ID3D12GraphicsCommandList* cmd, float w, float h) noexcept;
    void SetScissor(ID3D12GraphicsCommandList* cmd, int w, int h) noexcept;
    void ClearRTV(ID3D12GraphicsCommandList* cmd, D3D12_CPU_DESCRIPTOR_HANDLE rtv, const float clearColor[4]) noexcept;
    void SetRenderTarget(ID3D12GraphicsCommandList* cmd, D3D12_CPU_DESCRIPTOR_HANDLE rtv) noexcept;
}