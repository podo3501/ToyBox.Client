#pragma once
#include <d3d12.h>

class CommandList;
struct IResourceReady 
{ 
    virtual ~IResourceReady() = default;
    virtual void OnReady(CommandList& cmd) = 0;
};

struct PendingTransition
{
    ID3D12Resource* resource{ nullptr };

    D3D12_RESOURCE_STATES before{};
    D3D12_RESOURCE_STATES after{};

    uint64_t fence = 0;

    IResourceReady* owner{ nullptr };
};