#pragma once
#include "GameClient/Service/Render/Desc/RenderState.h"
#include <d3d12.h>
#include <wrl.h>

class ShaderProvider;

class PipelineCache
{
public:
    ~PipelineCache();
    PipelineCache();

    void Initialize(ID3D12Device* device, ShaderProvider* shaderProvider);
    ID3D12PipelineState* GetOrCreate(
        const PipelineState& pipelineState,
        ID3D12RootSignature* rootSignature,
        const std::function<void(D3D12_GRAPHICS_PIPELINE_STATE_DESC&)>& setup);
    ID3D12PipelineState* Find(const PipelineState& pipelineState);

private:
    ID3D12Device* m_device{ nullptr };
    ShaderProvider* m_shaderProvider{ nullptr };

    std::unordered_map<PipelineState, Microsoft::WRL::ComPtr<ID3D12PipelineState>, PipelineStateHasher> m_cache;
};