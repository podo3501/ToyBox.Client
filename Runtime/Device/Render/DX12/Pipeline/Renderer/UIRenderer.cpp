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
    const cm::Matrix& quadWorld,
    const std::optional<Rect>& source)
{
    auto drawCBAddress = UploadDrawCB(material, quadWorld, source);

    auto texIndices = material.GetTextureIndices();
    uint32_t resIndices[3] = { mesh.GetVertexHeapIndex(), mesh.GetIndexHeapIndex(), texIndices[0] };

    cmd->SetGraphicsRoot32BitConstants(Core::ToIndex(RootSlot::ResourceIndices), 3, resIndices, 0);
    cmd->SetGraphicsRootConstantBufferView(Core::ToIndex(RootSlot::DrawCB), drawCBAddress);

    cmd->DrawInstanced(mesh.GetIndexCount(), 1, 0, 0);
}

D3D12_GPU_VIRTUAL_ADDRESS UIRenderer::UploadDrawCB(
    UIMaterialResource& material,
    const Core::Math::Matrix& world,
    const std::optional<Rect>& source)
{
    UIDrawCB drawCB{};

    DirectX::XMMATRIX xmWorld = ToDXMatrix(world);
    XMStoreFloat4x4(&drawCB.world, DirectX::XMMatrixTranspose(xmWorld));

    DirectX::XMMATRIX xmProj = ToDXMatrix(m_projection);
    XMStoreFloat4x4(&drawCB.projection, DirectX::XMMatrixTranspose(xmProj));

    const auto& uv = material.CalcUVTransform(source);
    drawCB.uvTransform = ToXMFLOAT4(uv);

    return m_uiDrawCBAllocator.AllocateConstant(drawCB);
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