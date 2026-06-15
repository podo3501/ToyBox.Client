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

struct UIFrameCB
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
    m_uiFrameCBAllocator.Initialize<UIFrameCB>(device, m_config.maxUI);

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

    builder.Add32BitConstants(0, 3); // [0]: 버텍스 버퍼 인덱스, [1]: 인덱스 버퍼 인덱스, [2]: UI 텍스처 인덱스
    builder.AddCBV(1);
    builder.AddLinearSampler(0);

    builder.AddFlags(D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    m_rootSignature = builder.Build(device);
    return m_rootSignature != nullptr;
}

void UIRenderer::BindCommonState(CommandList& cmd)
{
    ID3D12DescriptorHeap* heaps[] = { m_srvHeap };
    cmd->SetDescriptorHeaps(1, heaps);

    cmd->SetGraphicsRootSignature(m_rootSignature.Get());
}

void UIRenderer::BindPipeline(CommandList& cmd, const PipelineState& pipelineState)
{
    if (m_pipelineState && *m_pipelineState == pipelineState)
        return;

    auto pipeline = GetPipeline(pipelineState);
    cmd->SetPipelineState(pipeline);
    cmd->IASetPrimitiveTopology(ToD3D12(pipelineState.topologyType));

    m_pipelineState = pipelineState;
}

void UIRenderer::PrepareFrame()
{
    m_pipelineState = std::nullopt;
    m_uiFrameCBAllocator.Reset();
}

void UIRenderer::Draw(
    CommandList& cmd,
    MeshResource& mesh,
    UIMaterialResource& material,
    const cm::Matrix& quadWorld)
{
    auto texIndices = material.GetTextureIndices();

    uint32_t uiIndices[3] = {
        mesh.GetVertexHeapIndex(),
        mesh.GetIndexHeapIndex(),
        texIndices[0]
    };

    cmd->SetGraphicsRoot32BitConstants(0, 3, uiIndices, 0);

    DirectX::XMMATRIX world = ToDXMatrix(quadWorld);
    DirectX::XMMATRIX proj = ToDXMatrix(m_projection);

    UIFrameCB frameCB{};

    XMStoreFloat4x4(&frameCB.world, DirectX::XMMatrixTranspose(world));
    XMStoreFloat4x4(&frameCB.projection, DirectX::XMMatrixTranspose(proj));

    auto gpuAddress = m_uiFrameCBAllocator.AllocateConstant(frameCB);
    cmd->SetGraphicsRootConstantBufferView(1, gpuAddress);

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