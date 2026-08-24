#pragma once
#include "../d3dx12.h"
#include <concepts>
#include "../Resource/Resource.h"

template<typename T>
concept CommandListLike = requires(T t)
{
    { t.Get() } -> std::same_as<ID3D12GraphicsCommandList*>;
};

namespace CommandUtils
{
    inline D3D12_RESOURCE_BARRIER CreateTransitionBarrier(
        const Resource& resource,
        D3D12_RESOURCE_STATES before,
        D3D12_RESOURCE_STATES after) noexcept
    {
        return CD3DX12_RESOURCE_BARRIER::Transition(
            resource.Get(),
            before,
            after);
    }

    template<CommandListLike T>
    inline void Transition(T& cmd, const Resource& resource,
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
    inline void SetViewport(T& cmd, float x, float y, float w, float h) noexcept
    {
        D3D12_VIEWPORT vp = {};
        vp.TopLeftX = x;
        vp.TopLeftY = y;
        vp.Width = w;
        vp.Height = h;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;

        cmd.Get()->RSSetViewports(1, &vp);
    }

    template<CommandListLike T>
    inline void SetScissor(T& cmd, int x, int y, int w, int h) noexcept
    {
        D3D12_RECT rect = {};
        rect.left = x;
        rect.top = y;
        rect.right = x + w;
        rect.bottom = y + h;

        cmd.Get()->RSSetScissorRects(1, &rect);
    }

    template<CommandListLike T>
    inline void SetScissor(T& cmd, float x, float y, float w, float h) noexcept
    {
        SetScissor(
            cmd,
            static_cast<int>(x),
            static_cast<int>(y),
            static_cast<int>(w),
            static_cast<int>(h));
    }

    template<CommandListLike T>
    void SetViewRect(T& cmd, const Rect& rect)
    {
        SetViewport(cmd, rect.x, rect.y, rect.width, rect.height);
        SetScissor(cmd, rect.x, rect.y, rect.width, rect.height);
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

    template<CommandListLike T>
    inline void SetRenderTarget(
        T& cmd, 
        D3D12_CPU_DESCRIPTOR_HANDLE rtv) noexcept
    {
        cmd.Get()->OMSetRenderTargets(1, &rtv, FALSE, nullptr);
    }

    template<CommandListLike T>
    inline void SetDepthTarget(
        T& cmd,
        D3D12_CPU_DESCRIPTOR_HANDLE dsv) noexcept
    {
        cmd.Get()->OMSetRenderTargets(0, nullptr, FALSE, &dsv);
    }
}