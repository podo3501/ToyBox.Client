#include "pch.h"
#include "UIRenderer.h"
#include "RootSignatureBuilder.h"
#include "../Command/CommandList.h"
#include "../MeshResource.h"
#include "../UIMaterialResource.h"
#include "../Helpers/MathHelpers.h"

namespace cm = Core::Math;

struct UIFrameCB
{
    DirectX::XMFLOAT4X4 world;
    DirectX::XMFLOAT4X4 projection;
};

using Microsoft::WRL::ComPtr;

UIRenderer::~UIRenderer() = default;
UIRenderer::UIRenderer(ID3D12Device* device, ShaderSystem* shaderSystem) :
    m_device{ device },
    m_shaderSystem{ shaderSystem }
{}

bool UIRenderer::Initialize(const Size& screenSize)
{
    m_pipelineCache.Initialize(m_device, m_shaderSystem);

    ReturnIfFalse(CreateRootSignature());
    CreateDefaultPSOs();
    m_uiFrameCBAllocator.Initialize(m_device, kMaxUI * kCBSize);

    SetScreenSize(screenSize);

    return true;
}

void UIRenderer::CreateDefaultPSOs()
{
    CreatePSO(PipelineLibrary::Get(ShaderID::UI, RasterPreset::NoCull));
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

bool UIRenderer::CreateRootSignature()
{
    RootSignatureBuilder builder;

    builder.AddSRVTable(2, 0);
    builder.AddSRVTable(1, 2);
    builder.AddCBV(1);
    builder.AddLinearSampler(0);

    m_rootSignature = builder.Build(m_device);
    return m_rootSignature != nullptr;
}

void UIRenderer::BindCommonState(CommandList& cmd)
{
    cmd->SetGraphicsRootSignature(m_rootSignature.Get());

    ID3D12DescriptorHeap* heaps[] = { m_srvHeap };
    cmd->SetDescriptorHeaps(1, heaps);
}

void UIRenderer::BindPipeline(CommandList& cmd, const PipelineState& pipelineState)
{
    cmd->SetPipelineState(GetPipeline(pipelineState));
    m_pipelineState = pipelineState;
}

void UIRenderer::PrepareFrame()
{
    m_uiFrameCBAllocator.Reset();
}

void UIRenderer::Draw(
    CommandList& cmd,
    MeshResource& mesh,
    UIMaterialResource& material,
    const cm::Matrix& quadWorld)
{
    auto& meshTable = mesh.GetMeshTable();
    auto& textureSrv = material.GetTextureSRV();

    cmd->SetGraphicsRootDescriptorTable(0, meshTable.GetGpuHandle());
    cmd->SetGraphicsRootDescriptorTable(1, textureSrv.GetGpuHandle());

    DirectX::XMMATRIX world = ToDXMatrix(quadWorld);
    DirectX::XMMATRIX proj = ToDXMatrix(m_projection);

    UIFrameCB frameCB{};

    XMStoreFloat4x4(&frameCB.world, DirectX::XMMatrixTranspose(world));
    XMStoreFloat4x4(&frameCB.projection, DirectX::XMMatrixTranspose(proj));

    auto gpuAddress = m_uiFrameCBAllocator.AllocateConstant(frameCB);
    cmd->SetGraphicsRootConstantBufferView(2, gpuAddress);

    switch (m_pipelineState.topologyType)
    {
    case PrimitiveTopologyType::Triangle:
        cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        break;

    case PrimitiveTopologyType::Line:
        cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
        break;
    }

    cmd->DrawInstanced(mesh.GetIndexCount(), 1, 0, 0);

    meshTable.MarkUsed(cmd.GetType(), cmd.GetFence());
    textureSrv.MarkUsed(cmd.GetType(), cmd.GetFence());
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