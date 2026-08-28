#include "pch.h"
#include "DebugSurfaceRenderer.h"
#include "PipelineCache.h"
#include "RootSignatureBuilder.h"
#include "Command/CommandList.h"
#include "RenderConstants.h"
#include "Helpers/MathHelpers.h"
#include "GameClient/Graphics/RenderData/CameraData.h"
#include "Resource/Mesh/MeshResource.h"
#include "Core/D3D12Conversions.h"
#include "Definition/RenderFormat.h"
#include "GameClient/Service/Render/Definition/Shader/RegistryShader.h"

DebugSurfaceRenderer::~DebugSurfaceRenderer() = default;
DebugSurfaceRenderer::DebugSurfaceRenderer(const DebugSurfaceRendererConfig& config, PipelineCache& pipelineCache) :
    m_config{ config },
    m_pipelineCache{ pipelineCache }
{}

bool DebugSurfaceRenderer::Initialize(Device& device)
{
    m_objectCBAllocator.Initialize<ObjectCB>(device, m_config.maxObjectCount);
    m_frameCBAllocator.Initialize<FrameCB>(device, m_config.maxViewCount);

    ReturnIfFalse(CreateRootSignature(device));
    ReturnIfFalse(CreateDefaultPSOs());

    return true;
}

bool DebugSurfaceRenderer::CreateRootSignature(Device& device)
{
    RootSignatureBuilder builder;

    builder.Add32BitConstants(Core::ToIndex(RootSlot::VertexIndex), 1);
    builder.AddCBV(Core::ToIndex(RootSlot::FrameCB));
    builder.AddCBV(Core::ToIndex(RootSlot::ObjectCB));

    builder.AddFlags(D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    m_rootSignature = builder.Build(device);
    return m_rootSignature != nullptr;
}

bool DebugSurfaceRenderer::CreateDefaultPSOs()
{
    ReturnIfFalse(CreatePSO(PipelineLibrary::Get(RegistryShader::Grid, RasterPreset::Default, PrimitiveTopologyType::Line)) != nullptr);

    return true;
}

void DebugSurfaceRenderer::ResetFrameResources()
{
    m_objectCBAllocator.Reset();
    m_frameCBAllocator.Reset();
}

void DebugSurfaceRenderer::PrepareDraw(
    CommandList& cmd,
    const CameraData& camera)
{
    m_currentPSO = nullptr;

    auto frameCBAddress = UploadFrameCB(camera);

    cmd->SetGraphicsRootSignature(m_rootSignature.Get());
    cmd->SetGraphicsRootConstantBufferView(Core::ToIndex(RootSlot::FrameCB), frameCBAddress);
}

void DebugSurfaceRenderer::BindPipeline(CommandList& cmd, const PipelineState& pipelineState)
{
    auto* pso = GetPipeline(pipelineState);
    if (m_currentPSO == pso)
        return;

    cmd->SetPipelineState(pso);
    cmd->IASetPrimitiveTopology(ToD3D12_Draw(pipelineState.topologyType));
    m_currentPSO = pso;
}

ID3D12PipelineState* DebugSurfaceRenderer::GetPipeline(const PipelineState& pipelineState)
{
    auto* pipeline = m_pipelineCache.Find(pipelineState);
    if (pipeline)
        return pipeline;

    return CreatePSO(pipelineState);
}

ID3D12PipelineState* DebugSurfaceRenderer::CreatePSO(const PipelineState& pipelineState)
{
    return m_pipelineCache.GetOrCreate(
        pipelineState,
        m_rootSignature.Get(),
        [&](D3D12_GRAPHICS_PIPELINE_STATE_DESC& pso)
        {
            pso.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
            pso.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
            pso.DSVFormat = RenderFormat::DepthFormat;
        });
}

void DebugSurfaceRenderer::Draw(CommandList& cmd, MeshResource& mesh, const Core::Matrix& world)
{
    auto objectCBAddress = UploadObjectCB(world);
    uint32_t vbIndex = mesh.GetVertexHeapIndex();

    cmd->SetGraphicsRoot32BitConstants(Core::ToIndex(RootSlot::VertexIndex), 1, &vbIndex, 0);
    cmd->SetGraphicsRootConstantBufferView(Core::ToIndex(RootSlot::ObjectCB), objectCBAddress);

    cmd->DrawInstanced(mesh.GetVertexCount(), 1, 0, 0);
}

D3D12_GPU_VIRTUAL_ADDRESS DebugSurfaceRenderer::UploadFrameCB(const CameraData& camera)
{
    FrameCB frame{};
    DirectX::XMMATRIX view = ToDXMatrix(camera.view);
    DirectX::XMMATRIX proj = ToDXMatrix(camera.proj);

    // GPU용으로 transpose해서 저장
    XMStoreFloat4x4(&frame.view, DirectX::XMMatrixTranspose(view));
    XMStoreFloat4x4(&frame.proj, DirectX::XMMatrixTranspose(proj));

    return m_frameCBAllocator.AllocateConstant(frame);
}

D3D12_GPU_VIRTUAL_ADDRESS DebugSurfaceRenderer::UploadObjectCB(const Core::Matrix& world)
{
    ObjectCB obj{};

    DirectX::XMMATRIX xmWorld = ToDXMatrix(world);
    XMStoreFloat4x4(&obj.world, DirectX::XMMatrixTranspose(xmWorld));

    return m_objectCBAllocator.AllocateConstant(obj);
}