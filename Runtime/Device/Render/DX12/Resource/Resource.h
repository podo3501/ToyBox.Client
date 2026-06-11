#pragma once
#include <wrl/client.h>
#include "../d3dx12.h"

using Microsoft::WRL::ComPtr;

class Resource
{
public:
    ~Resource();
    Resource() = default;
    explicit Resource(ComPtr<ID3D12Resource> resource);
    explicit operator bool() const noexcept { return m_resource != nullptr; }
    void Reset() noexcept { m_resource.Reset(); }

    ID3D12Resource* operator->() const { return m_resource.Get(); }
    ID3D12Resource* Get() const { return m_resource.Get(); }
    ID3D12Resource** GetAddressOf() { return m_resource.GetAddressOf(); }

private:
    ComPtr<ID3D12Resource> m_resource;
};