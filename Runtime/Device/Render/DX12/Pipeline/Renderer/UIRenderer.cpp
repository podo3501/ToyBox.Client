#include "pch.h"
#include "UIRenderer.h"
#include "PipelineCache.h"
#include "RootSignatureBuilder.h"
#include "Command/CommandList.h"
#include "Resource/Mesh/MeshResource.h"
#include "Resource/Material/UIMaterialResource.h"
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

bool UIRenderer::Initialize(Device& device, const Size& screenSize)
{
    m_uiDrawCBAllocator.Initialize<UIDrawCB>(device, m_config.maxUI);

    ReturnIfFalse(CreateRootSignature(device));
    ReturnIfFalse(CreateDefaultPSOs());
    SetScreenSize(screenSize);

    return true;
}

bool UIRenderer::CreateDefaultPSOs()
{
    ReturnIfFalse(CreatePSO(PipelineLibrary::Get(RegistryShader::UI, RasterPreset::NoCull)) != nullptr);
    ReturnIfFalse(CreatePSO(PipelineLibrary::Get(RegistryShader::Text, RasterPreset::NoCull)) != nullptr);

    return true;
}

ID3D12PipelineState* UIRenderer::CreatePSO(const PipelineState& pipelineState)
{
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

            // 컬러 블렌딩 공식: (정답 색상 * 글자 알파) + (바탕 화면 색상 * (1 - 글자 알파))
            rtBlend.SrcBlend = D3D12_BLEND_SRC_ALPHA;
            rtBlend.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
            rtBlend.BlendOp = D3D12_BLEND_OP_ADD;

            // 알파 채널 자체의 블렌딩 공식
            rtBlend.SrcBlendAlpha = D3D12_BLEND_ONE;
            rtBlend.DestBlendAlpha = D3D12_BLEND_ZERO;
            rtBlend.BlendOpAlpha = D3D12_BLEND_OP_ADD;

            rtBlend.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
        });
}

ID3D12PipelineState* UIRenderer::GetPipeline(const PipelineState& pipelineState)
{
    auto* pipeline = m_pipelineCache.Find(pipelineState);
    if (pipeline)
        return pipeline;

    return CreatePSO(pipelineState);
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

void UIRenderer::PrepareFrame()
{
    m_uiDrawCBAllocator.Reset();
}

void UIRenderer::BeginFrame(CommandList& cmd)
{
    m_currentPSO = nullptr;
    cmd->SetGraphicsRootSignature(m_rootSignature.Get());
}

void UIRenderer::BindPipeline(CommandList& cmd, const PipelineState& pipelineState)
{
    auto* pso = GetPipeline(pipelineState);
    if (m_currentPSO == pso)
        return;

    cmd->SetPipelineState(pso);
    cmd->IASetPrimitiveTopology(ToD3D12_Draw(pipelineState.topologyType));
    m_currentPSO = pso;
}

void UIRenderer::Draw(
    CommandList& cmd,
    MeshResource& mesh,
    UIMaterialResource& material,
    const Core::Matrix& quadWorld,
    const Core::Vector4& uvTransform)
{
    auto drawCBAddress = UploadDrawCB(material, quadWorld, uvTransform);

    auto texIndices = material.GetTextureIndices();
    uint32_t resIndices[3] = { mesh.GetVertexHeapIndex(), mesh.GetIndexHeapIndex(), texIndices[0] };

    cmd->SetGraphicsRoot32BitConstants(Core::ToIndex(RootSlot::ResourceIndices), 3, resIndices, 0);
    cmd->SetGraphicsRootConstantBufferView(Core::ToIndex(RootSlot::DrawCB), drawCBAddress);

    cmd->DrawInstanced(mesh.GetIndexCount(), 1, 0, 0);
}

D3D12_GPU_VIRTUAL_ADDRESS UIRenderer::UploadDrawCB(
    UIMaterialResource& material,
    const Core::Matrix& world,
    const Core::Vector4& uvTransform)
{
    UIDrawCB drawCB{};

    DirectX::XMMATRIX xmWorld = ToDXMatrix(world);
    XMStoreFloat4x4(&drawCB.world, DirectX::XMMatrixTranspose(xmWorld));

    DirectX::XMMATRIX xmProj = ToDXMatrix(m_projection);
    XMStoreFloat4x4(&drawCB.projection, DirectX::XMMatrixTranspose(xmProj));

    drawCB.uvTransform = ToXMFLOAT4(uvTransform);
    return m_uiDrawCBAllocator.AllocateConstant(drawCB);
}

void UIRenderer::SetScreenSize(const Size& size)
{
    m_projection = Core::Matrix::OrthographicOffCenter(
        0.0f,
        static_cast<float>(size.width),
        static_cast<float>(size.height),
        0.0f,
        0.0f,
        1.0f
    );
}