#pragma once
#include <d3d12.h>

struct IResourceReady 
{ 
    virtual ~IResourceReady() = default;
    virtual void OnReady(ID3D12GraphicsCommandList* cmd) = 0;
};

struct PendingTransition
{
    ID3D12Resource* resource{ nullptr };

    D3D12_RESOURCE_STATES before{};
    D3D12_RESOURCE_STATES after{};

    uint64_t fence = 0;

    IResourceReady* owner{ nullptr };
};