#include "pch.h"
#include "CompositeRenderer.h"
#include "PipelineCache.h"
#include "RootSignatureBuilder.h"
#include "PipelineStateUtils.h"
#include "Core/RenderFormat.h"
#include "Command/CommandList.h"

CompositeRenderer::~CompositeRenderer() = default;
CompositeRenderer::CompositeRenderer(PipelineCache& pipelineCache) :
    m_pipelineCache{ pipelineCache }
{}

bool CompositeRenderer::Initialize(Device& device)
{
    ReturnIfFalse(CreateRootSignature(device));
    m_compositePSO = CreatePSO(PipelineLibrary::Get(RegistryShader::Composite, RasterPreset::Default));
    if (!m_compositePSO) return false;

    return true;
}

bool CompositeRenderer::CreateRootSignature(Device& device)
{
    RootSignatureBuilder builder;

    // 뷰 컬러 타겟의 bindless SRV 힙 인덱스 하나만 넘김 (뷰 = 픽셀 셰이더에서 ResourceDescriptorHeap[index]로 접근)
    builder.Add32BitConstants(Core::ToIndex(RootSlot::CompositeData), 1);
    builder.AddLinearSampler(0, D3D12_TEXTURE_ADDRESS_MODE_CLAMP);

    m_rootSignature = builder.Build(device);
    return m_rootSignature != nullptr;
}

ID3D12PipelineState* CompositeRenderer::CreatePSO(const PipelineState& pipelineState)
{
    return m_pipelineCache.GetOrCreate(
        pipelineState,
        m_rootSignature.Get(),
        [&](D3D12_GRAPHICS_PIPELINE_STATE_DESC& pso)
        {
            pso.NumRenderTargets = 1;
            pso.RTVFormats[0] = RenderFormat::BackBufferSRGBView;

            // 뷰 타겟을 백버퍼 위에 그대로 옮겨 그리는 것뿐이므로 깊이 테스트 불필요
            pso.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
            pso.DepthStencilState.DepthEnable = FALSE;
            pso.DepthStencilState.StencilEnable = FALSE;
            pso.DSVFormat = DXGI_FORMAT_UNKNOWN;

            pso.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
            pso.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

            pso.InputLayout = { nullptr, 0 }; // 버텍스 버퍼 없이 SV_VertexID로 생성
            pso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

            SetPremultipliedAlphaBlend(pso.BlendState.RenderTarget[0]); //PMA로 설정.
        });
}

void CompositeRenderer::PrepareDraw(CommandList& cmd)
{
    cmd->SetGraphicsRootSignature(m_rootSignature.Get());
    cmd->SetPipelineState(m_compositePSO);
    cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void CompositeRenderer::Draw(CommandList& cmd, UINT colorSRVIndex)
{
    cmd->SetGraphicsRoot32BitConstant(
        Core::ToIndex(RootSlot::CompositeData), colorSRVIndex, 0);

    // 정점/인덱스 버퍼 없이 셰이더 내부에서 SV_VertexID 기반으로 풀스크린 삼각형 3정점 생성
    cmd->DrawInstanced(3, 1, 0, 0);
}