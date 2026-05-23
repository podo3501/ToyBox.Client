#pragma once
#include "../d3dx12.h"
#include <concepts>

template<typename T>
concept CommandListLike = requires(T t)
{
    { t.Get() } -> std::same_as<ID3D12GraphicsCommandList*>;
};

namespace CommandUtils
{
    inline D3D12_RESOURCE_BARRIER CreateTransitionBarrier(
        ID3D12Resource* resource,
        D3D12_RESOURCE_STATES before,
        D3D12_RESOURCE_STATES after) noexcept
    {
        return CD3DX12_RESOURCE_BARRIER::Transition(
            resource,
            before,
            after);
    }

    template<CommandListLike T>
    inline void Transition(T& cmd, ID3D12Resource* resource,
        D3D12_RESOURCE_STATES before,
        D3D12_RESOURCE_STATES after) noexcept
    {
        if (before == after)
            return;

        auto barrier = CreateTransitionBarrier(resource, before, after);
        cmd.Get()->ResourceBarrier(1, &barrier);
    }

    template<CommandListLike T>
    inline void Transition(
        T& cmd,
        const std::vector<D3D12_RESOURCE_BARRIER>& barriers) noexcept
    {
        if (barriers.empty())
            return;

        cmd.Get()->ResourceBarrier(
            static_cast<UINT>(barriers.size()),
            barriers.data());
    }

    template<CommandListLike T>
    inline void SetViewport(T& cmd, float w, float h) noexcept
    {
        D3D12_VIEWPORT vp = {};
        vp.TopLeftX = 0.0f;
        vp.TopLeftY = 0.0f;
        vp.Width = w;
        vp.Height = h;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;

        cmd.Get()->RSSetViewports(1, &vp);
    }

    template<CommandListLike T>
    inline void SetScissor(T& cmd, int w, int h) noexcept
    {
        D3D12_RECT rect = {};
        rect.left = 0;
        rect.top = 0;
        rect.right = w;
        rect.bottom = h;

        cmd.Get()->RSSetScissorRects(1, &rect);
    }

    template<CommandListLike T>
    inline void ClearRTV(T& cmd, D3D12_CPU_DESCRIPTOR_HANDLE rtv, const float clearColor[4]) noexcept
    {
        cmd.Get()->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
    }

    template<CommandListLike T>
    inline void ClearDSV(
        T& cmd,
        D3D12_CPU_DESCRIPTOR_HANDLE dsv,
        float depth = 1.0f,
        UINT8 stencil = 0,
        D3D12_CLEAR_FLAGS flags = D3D12_CLEAR_FLAG_DEPTH) noexcept
    {
        cmd.Get()->ClearDepthStencilView(dsv, flags, depth, stencil, 0, nullptr);
    }

    template<CommandListLike T>
    inline void SetRenderTarget(
        T& cmd, 
        D3D12_CPU_DESCRIPTOR_HANDLE rtv,
        D3D12_CPU_DESCRIPTOR_HANDLE dsv) noexcept
    {
        cmd.Get()->OMSetRenderTargets(1, &rtv, FALSE, &dsv);
    }
}