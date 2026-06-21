#include "pch.h"
#include "UIRenderer.h"
#include "PipelineCache.h"
#include "RootSignatureBuilder.h"
#include "Command/CommandList.h"
#include "Resource/Mesh/MeshResource.h"
#include "Resource/Material/UIMaterialResource.h"
#include "Helpers/MathHelpers.h"
#include "Core/D3D12Conversions.h"

namespace cm = Core::Math;

struct UIDrawCB
{
    DirectX::XMFLOAT4X4 world;
    DirectX::XMFLOAT4X4 projection;
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
    CreateDefaultPSOs();
    SetScreenSize(screenSize);

    return true;
}

void UIRenderer::CreateDefaultPSOs()
{
    CreatePSO(PipelineLibrary::Get(ShadingModel::UI, RasterPreset::NoCull));
}

ID3D12PipelineState* UIRenderer::CreatePSO(const PipelineState& pipelineState)
{
    return m_pipelineCache.GetOrCreate(
        pipelineState,
        m_rootSignature.Get(),
        [&](D3D12_GRAPHICS_PIPELINE_STATE_DESC& pso)
        {
            pso.DepthStencilState.DepthEnable = FALSE;
            pso.DepthStencilState.StencilEnable = FALSE;
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
    m_currentPSO = nullptr;
    m_uiDrawCBAllocator.Reset();
}

void UIRenderer::BeginFrame(CommandList& cmd)
{
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
    const cm::Matrix& quadWorld)
{
    auto texIndices = material.GetTextureIndices();

    uint32_t resIndices[3] = {
        mesh.GetVertexHeapIndex(),
        mesh.GetIndexHeapIndex(),
        texIndices[0]
    };

    cmd->SetGraphicsRoot32BitConstants(Core::ToIndex(RootSlot::ResourceIndices), 3, resIndices, 0);

    DirectX::XMMATRIX world = ToDXMatrix(quadWorld);
    DirectX::XMMATRIX proj = ToDXMatrix(m_projection);

    UIDrawCB drawCB{};

    XMStoreFloat4x4(&drawCB.world, DirectX::XMMatrixTranspose(world));
    XMStoreFloat4x4(&drawCB.projection, DirectX::XMMatrixTranspose(proj));

    auto gpuAddress = m_uiDrawCBAllocator.AllocateConstant(drawCB);
    cmd->SetGraphicsRootConstantBufferView(Core::ToIndex(RootSlot::DrawCB), gpuAddress);

    cmd->DrawInstanced(mesh.GetIndexCount(), 1, 0, 0);
}

void UIRenderer::SetScreenSize(const Size& size)
{
    m_projection = cm::Matrix::OrthographicOffCenter(
        0.0f,
        static_cast<float>(size.width),
        static_cast<float>(size.height),
        0.0f,
        0.0f,
        1.0f
    );
}