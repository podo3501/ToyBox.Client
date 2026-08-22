#include "pch.h"
#include "UIRenderer.h"
#include "PipelineCache.h"
#include "RootSignatureBuilder.h"
#include "Command/CommandList.h"
#include "Resource/Mesh/MeshResource.h"
#include "Resource/Brush/BrushResource.h"
#include "Helpers/MathHelpers.h"
#include "Core/D3D12Conversions.h"

struct UIDrawCB
{
    DirectX::XMFLOAT4X4 world;
    DirectX::XMFLOAT4X4 projection;
    DirectX::XMFLOAT4 uvTransform; //x=u0, y=v0, z=u1, w=v1
};

using Microsoft::WRL::ComPtr;

UIRenderer::~UIRenderer() = default;
UIRenderer::UIRenderer(const UIRendererConfig& config, PipelineCache& pipelineCache) :
    m_config{ config },
    m_pipelineCache{ pipelineCache }
{}

bool UIRenderer::Initialize(Device& device)
{
    m_uiDrawCBAllocator.Initialize<UIDrawCB>(device, m_config.maxUI);

    ReturnIfFalse(CreateRootSignature(device));
    m_pso = CreatePSO();
    if (!m_pso) return false;

    return true;
}

ID3D12PipelineState* UIRenderer::CreatePSO()
{
    PipelineState pipelineState = PipelineLibrary::Get(
        RegistryShader::UI,
        RasterPreset::NoCull);

    return m_pipelineCache.GetOrCreate(
        pipelineState,
        m_rootSignature.Get(),
        [&](D3D12_GRAPHICS_PIPELINE_STATE_DESC& pso)
        {
            pso.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
            pso.DepthStencilState.DepthEnable = FALSE;
            pso.DepthStencilState.StencilEnable = FALSE;

            //  알파 블렌딩(Alpha Blending) 설정 추가
            D3D12_RENDER_TARGET_BLEND_DESC& rtBlend = pso.BlendState.RenderTarget[0];
            rtBlend.BlendEnable = TRUE;
            rtBlend.LogicOpEnable = FALSE;

            // PMA 컬러 블렌딩 공식: (셰이더가 낸 rgb, 이미 alpha가 곱해진 상태) + (배경 * (1 - alpha))
            // Straight일 때는 SRC_ALPHA * rgb를 GPU가 곱해줬는데,
            // PMA는 셰이더가 이미 rgb*alpha를 계산해서 내놓으므로 GPU가 또 곱하면 안 됨 -> ONE
            rtBlend.SrcBlend = D3D12_BLEND_ONE;
            rtBlend.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
            rtBlend.BlendOp = D3D12_BLEND_OP_ADD;

            // 알파 채널 자체의 블렌딩 공식
            rtBlend.SrcBlendAlpha = D3D12_BLEND_ONE;
            rtBlend.DestBlendAlpha = D3D12_BLEND_ZERO;
            rtBlend.BlendOpAlpha = D3D12_BLEND_OP_ADD;

            rtBlend.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
        });
}

bool UIRenderer::CreateRootSignature(Device& device)
{
    RootSignatureBuilder builder;

    builder.Add32BitConstants(Core::ToIndex(RootSlot::ResourceIndices), 3);
    builder.AddCBV(Core::ToIndex(RootSlot::DrawCB));
    builder.AddLinearSampler(0);

    builder.AddFlags(D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    m_rootSignature = builder.Build(device);
    return m_rootSignature != nullptr;
}

void UIRenderer::BeginFrame(CommandList& cmd)
{
    m_uiDrawCBAllocator.Reset();

    cmd->SetGraphicsRootSignature(m_rootSignature.Get());
    cmd->SetPipelineState(m_pso);
    cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void UIRenderer::Draw(
    CommandList& cmd,
    MeshResource& mesh,
    BrushResource& brush,
    const Core::Matrix& quadWorld,
    const Core::Matrix& projection,
    const std::optional<Rect>& source)
{
    const Core::Vector4 uvTransform = brush.CalcUVTransform(source);
    auto drawCBAddress = UploadDrawCB( quadWorld, projection, uvTransform);

    uint32_t resIndices[3] = 
    { 
        mesh.GetVertexHeapIndex(), 
        mesh.GetIndexHeapIndex(), 
        brush.GetTextureIndex() 
    };

    cmd->SetGraphicsRoot32BitConstants(Core::ToIndex(RootSlot::ResourceIndices), 3, resIndices, 0);
    cmd->SetGraphicsRootConstantBufferView(Core::ToIndex(RootSlot::DrawCB), drawCBAddress);

    cmd->DrawInstanced(mesh.GetIndexCount(), 1, 0, 0);
}

D3D12_GPU_VIRTUAL_ADDRESS UIRenderer::UploadDrawCB(
    const Core::Matrix& world,
    const Core::Matrix& projection,
    const Core::Vector4& uvTransform)
{
    UIDrawCB drawCB{};

    DirectX::XMMATRIX xmWorld = ToDXMatrix(world);
    XMStoreFloat4x4(&drawCB.world, DirectX::XMMatrixTranspose(xmWorld));

    DirectX::XMMATRIX xmProj = ToDXMatrix(projection);
    XMStoreFloat4x4(&drawCB.projection, DirectX::XMMatrixTranspose(xmProj));

    drawCB.uvTransform = ToXMFLOAT4(uvTransform);
    return m_uiDrawCBAllocator.AllocateConstant(drawCB);
}