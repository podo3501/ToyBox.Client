#pragma once

struct ID3D12Device;
struct ID3D12CommandQueue;
struct IDXGIFactory4;

struct DX12DeviceView
{
    ID3D12Device* device{ nullptr };
    IDXGIFactory4* factory{ nullptr };
    ID3D12CommandQueue* queue{ nullptr };
    ID3D12CommandQueue* copyQueue{ nullptr };
};
