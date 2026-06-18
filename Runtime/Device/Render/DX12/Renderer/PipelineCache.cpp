#include "pch.h"
#include "PipelineCache.h"
#include "Core/Device.h"
#include "Core/D3D12Conversions.h"
#include "Resource/Shader/ShaderProvider.h"
#include "../d3dx12.h"

PipelineCache::~PipelineCache() = default;
PipelineCache::PipelineCache(Device& device, ShaderProvider& shaderProvider) :
    m_device{ device },
    m_shaderProvider{ shaderProvider }
{}

ID3D12PipelineState* PipelineCache::GetOrCreate(
    const PipelineState& pipelineState,
    ID3D12RootSignature* rootSignature,
    const std::function<void(D3D12_GRAPHICS_PIPELINE_STATE_DESC&)>& setup)
{
    auto it = m_cache.find(pipelineState);
    if (it != m_cache.end())
        return it->second.Get();

    const ShaderEntry* shaderEntry = m_shaderProvider.Find(pipelineState.shaderVariant);
    if (!shaderEntry)
        return nullptr;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC pso{};

    pso.InputLayout = { nullptr, 0 };
    pso.pRootSignature = rootSignature;
    pso.VS = { shaderEntry->vs->GetBufferPointer(), shaderEntry->vs->GetBufferSize() };
    pso.PS = { shaderEntry->ps->GetBufferPointer(), shaderEntry->ps->GetBufferSize() };

    CD3DX12_RASTERIZER_DESC raster(D3D12_DEFAULT);

    raster.FillMode = ToD3D12(pipelineState.rasterState.fillMode);
    raster.CullMode = ToD3D12(pipelineState.rasterState.cullMode);

    pso.RasterizerState = raster;
    pso.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    pso.SampleMask = UINT_MAX;
    pso.PrimitiveTopologyType = ToD3D12_PSO(pipelineState.topologyType);
    pso.NumRenderTargets = 1;
    pso.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    pso.SampleDesc.Count = 1;

    setup(pso);

    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipeline;

    auto result = m_device->CreateGraphicsPipelineState(&pso, IID_PPV_ARGS(&pipeline));
    Assert(SUCCEEDED(result));
    m_cache[pipelineState] = pipeline;

    return pipeline.Get();
}

ID3D12PipelineState* PipelineCache::Find(const PipelineState& pipelineState)
{
    auto it = m_cache.find(pipelineState);
    if (it == m_cache.end())
        return nullptr;

    return it->second.Get();
}