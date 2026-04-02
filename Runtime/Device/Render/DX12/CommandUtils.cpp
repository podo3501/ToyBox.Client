#include "pch.h"
#include "CommandUtils.h"

void CommandUtils::Transition(
    ID3D12GraphicsCommandList* cmd,
    ID3D12Resource* resource,
    D3D12_RESOURCE_STATES before,
    D3D12_RESOURCE_STATES after) noexcept
{
    if (before == after) return;

    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource, before, after);
    cmd->ResourceBarrier(1, &barrier);
}

void CommandUtils::SetViewport(ID3D12GraphicsCommandList* cmd, float w, float h) noexcept
{
    D3D12_VIEWPORT vp = {};
    vp.TopLeftX = 0.0f;
    vp.TopLeftY = 0.0f;
    vp.Width = w;
    vp.Height = h;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;

    cmd->RSSetViewports(1, &vp);
}

void CommandUtils::SetScissor(ID3D12GraphicsCommandList* cmd, int w, int h) noexcept
{
    D3D12_RECT rect = {};
    rect.left = 0;
    rect.top = 0;
    rect.right = w;
    rect.bottom = h;

    cmd->RSSetScissorRects(1, &rect);
}

void CommandUtils::ClearRTV(ID3D12GraphicsCommandList* cmd, D3D12_CPU_DESCRIPTOR_HANDLE rtv, const float clearColor[4]) noexcept
{
    cmd->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
}

void CommandUtils::SetRenderTarget(ID3D12GraphicsCommandList* cmd, D3D12_CPU_DESCRIPTOR_HANDLE rtv) noexcept
{
    cmd->OMSetRenderTargets(1, &rtv, FALSE, nullptr);
}