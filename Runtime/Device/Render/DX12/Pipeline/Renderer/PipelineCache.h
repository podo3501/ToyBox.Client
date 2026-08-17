#pragma once
#include "GameClient/Service/Render/Definition/View/RenderState.h"
#include <d3d12.h>
#include <wrl.h>

class Device;
class ShaderLibrary;

class PipelineCache
{
public:
    ~PipelineCache();
    PipelineCache(Device& device, ShaderLibrary& shaderLibaray);

    ID3D12PipelineState* GetOrCreate(
        const PipelineState& pipelineState,
        ID3D12RootSignature* rootSignature,
        const std::function<void(D3D12_GRAPHICS_PIPELINE_STATE_DESC&)>& setup);
    ID3D12PipelineState* Find(const PipelineState& pipelineState);

private:
    Device& m_device;
    ShaderLibrary& m_shaderLibrary;

    std::unordered_map<PipelineState, Microsoft::WRL::ComPtr<ID3D12PipelineState>, PipelineStateHasher> m_cache;
};