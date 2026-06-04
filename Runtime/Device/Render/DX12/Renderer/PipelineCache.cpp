#include "pch.h"
#include "PipelineCache.h"
#include "../ShaderSystem.h"
#include "../d3dx12.h"

PipelineCache::~PipelineCache() = default;
PipelineCache::PipelineCache() = default;

void PipelineCache::Initialize(ID3D12Device* device, ShaderSystem* shaderSystem)
{
    m_device = device;
    m_shaderSystem = shaderSystem;
}

ID3D12PipelineState* PipelineCache::GetOrCreate(
    const PipelineState& pipelineState,
    ID3D12RootSignature* rootSignature,
    const std::function<void(D3D12_GRAPHICS_PIPELINE_STATE_DESC&)>& setup)
{
    auto it = m_cache.find(pipelineState);
    if (it != m_cache.end())
        return it->second.Get();

    const ShaderEntry* shaderEntry = m_shaderSystem->Find(pipelineState.shaderVariant);
    if (!shaderEntry)
        return nullptr;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC pso{};

    pso.InputLayout = { nullptr, 0 };
    pso.pRootSignature = rootSignature;
    pso.VS = { shaderEntry->vs->GetBufferPointer(), shaderEntry->vs->GetBufferSize() };
    pso.PS = { shaderEntry->ps->GetBufferPointer(), shaderEntry->ps->GetBufferSize() };

    CD3DX12_RASTERIZER_DESC raster(D3D12_DEFAULT);

    raster.FillMode =
        pipelineState.rasterState.fillMode == FillMode::Wireframe
        ? D3D12_FILL_MODE_WIREFRAME
        : D3D12_FILL_MODE_SOLID;

    switch (pipelineState.rasterState.cullMode)
    {
    case CullMode::None: raster.CullMode = D3D12_CULL_MODE_NONE; break;
    case CullMode::Front: raster.CullMode = D3D12_CULL_MODE_FRONT; break;
    case CullMode::Back: raster.CullMode = D3D12_CULL_MODE_BACK; break;
    }

    pso.RasterizerState = raster;
    pso.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    pso.SampleMask = UINT_MAX;

    switch (pipelineState.topologyType)
    {
    case PrimitiveTopologyType::Triangle: pso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; break;
    case PrimitiveTopologyType::Line: pso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE; break;
    }

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