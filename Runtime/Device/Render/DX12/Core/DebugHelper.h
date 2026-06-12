#pragma once
#include <wrl/client.h>
#include <d3d12.h>

using Microsoft::WRL::ComPtr;

struct DebugOptions;

class DebugHelper
{
public:
    static void EnableDebugLayer(const DebugOptions& opt);
    static void SetupInfoQueue(ID3D12Device* device, const DebugOptions& opt);
};