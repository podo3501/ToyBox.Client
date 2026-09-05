#include "pch.h"
#include "UIRenderer.h"
#include "PipelineCache.h"
#include "RootSignatureBuilder.h"
#include "PipelineStateUtils.h"
#include "Command/CommandList.h"
#include "Resource/Mesh/MeshResource.h"
#include "Helpers/MathHelpers.h"
#include "GameClient/Service/Render/Definition/Shader/RegistryShader.h"

struct UIDrawCB
{
    DirectX::XMFLOAT4X4 projection;
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
            pso.DSVFormat = DXGI_FORMAT_UNKNOWN;

            SetPremultipliedAlphaBlend(pso.BlendState.RenderTarget[0]); //PMA로 설정.
        });
}

bool UIRenderer::CreateRootSignature(Device& device)
{
    RootSignatureBuilder builder;

    builder.Add32BitConstants(Core::ToIndex(RootSlot::ResourceIndices), 2);
    builder.AddCBV(Core::ToIndex(RootSlot::DrawCB));
    builder.AddLinearSampler(0);

    builder.AddFlags(D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    m_rootSignature = builder.Build(device);
    return m_rootSignature != nullptr;
}

void UIRenderer::ResetFrameResources()
{
    m_uiDrawCBAllocator.Reset();
}

void UIRenderer::BeginFrame(CommandList& cmd)
{
    cmd->SetGraphicsRootSignature(m_rootSignature.Get());
    cmd->SetPipelineState(m_pso);
    cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void UIRenderer::Draw(
    CommandList& cmd,
    MeshResource& mesh,
    const Core::Matrix& projection)
{
    uint32_t resIndices[2] = 
    { 
        mesh.GetVertexHeapIndex(), 
        mesh.GetIndexHeapIndex() 
    };
    auto drawCBAddress = UploadDrawCB(projection);

    cmd->SetGraphicsRoot32BitConstants(Core::ToIndex(RootSlot::ResourceIndices), 2, resIndices, 0);
    cmd->SetGraphicsRootConstantBufferView(Core::ToIndex(RootSlot::DrawCB), drawCBAddress);

    cmd->DrawInstanced(mesh.GetIndexCount(), 1, 0, 0);
}

D3D12_GPU_VIRTUAL_ADDRESS UIRenderer::UploadDrawCB(const Core::Matrix& projection)
{
    UIDrawCB drawCB{};
    DirectX::XMMATRIX xmProj = ToDXMatrix(projection);
    XMStoreFloat4x4(&drawCB.projection, DirectX::XMMatrixTranspose(xmProj));
    return m_uiDrawCBAllocator.AllocateConstant(drawCB);
}