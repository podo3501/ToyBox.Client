#include "pch.h"
#include "InspectorImageRenderer.h"
#include "Definition/RenderFormat.h"
#include "Pipeline/Renderer/PipelineCache.h"
#include "Pipeline/Renderer/RootSignatureBuilder.h"
#include "Command/CommandList.h"
#include "Helpers/MathHelpers.h"
#include "Core/D3D12Conversions.h"
#include "GameClient/Service/Render/Definition/Shader/RegistryShader.h"

struct InspectorTextureCB
{
    uint32_t srvIndex;
};

struct InspectorDrawCB
{
    DirectX::XMFLOAT4X4 world;
    DirectX::XMFLOAT4X4 projection;

    DirectX::XMFLOAT2 imageSize;
    DirectX::XMFLOAT2 padding{};
};

InspectorImageRenderer::~InspectorImageRenderer() = default;
InspectorImageRenderer::InspectorImageRenderer(PipelineCache& pipelineCache) noexcept :
    m_pipelineCache{ pipelineCache }
{
    m_pipelineState = PipelineLibrary::Get(RegistryShader::InspectorImage, RasterPreset::NoCull);
}

bool InspectorImageRenderer::Initialize(Device& device, const Size& screenSize)
{
    m_drawCBAllocator.Initialize<InspectorDrawCB>(device, MaxImage);

    ReturnIfFalse(CreateRootSignature(device));
    ReturnIfFalse(CreateDefaultPSOs());
    SetScreenSize(screenSize);

    return true;
}

bool InspectorImageRenderer::CreateDefaultPSOs()
{
    ReturnIfFalse(CreatePSO(m_pipelineState) != nullptr);

    return true;
}

ID3D12PipelineState* InspectorImageRenderer::CreatePSO(const PipelineState& pipelineState)
{
    return m_pipelineCache.GetOrCreate(
        pipelineState,
        m_rootSignature.Get(),
        [&](D3D12_GRAPHICS_PIPELINE_STATE_DESC& pso)
        {
            pso.NumRenderTargets = 1;
            pso.RTVFormats[0] = RenderFormat::BackBufferSRGBView;

            pso.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
            pso.DepthStencilState.DepthEnable = FALSE;
            pso.DepthStencilState.StencilEnable = FALSE;

            auto& rtBlend = pso.BlendState.RenderTarget[0];

            rtBlend.BlendEnable = FALSE;
            rtBlend.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
        });
}

ID3D12PipelineState* InspectorImageRenderer::GetPipeline(const PipelineState& pipelineState)
{
    auto* pipeline = m_pipelineCache.Find(pipelineState);
    if (pipeline)
        return pipeline;

    return CreatePSO(pipelineState);
}

bool InspectorImageRenderer::CreateRootSignature(Device& device)
{
    RootSignatureBuilder builder;

    builder.Add32BitConstants(Core::ToIndex(RootSlot::ResourceIndices), 1);
    builder.AddCBV(Core::ToIndex(RootSlot::DrawCB));
    builder.AddPointSampler(0);

    builder.AddFlags(D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    m_rootSignature = builder.Build(device);
    return m_rootSignature != nullptr;
}

void InspectorImageRenderer::PrepareFrame()
{
    m_drawCBAllocator.Reset();
}

void InspectorImageRenderer::BeginFrame(CommandList& cmd)
{
    m_currentPSO = nullptr;
    cmd->SetGraphicsRootSignature(m_rootSignature.Get());
}

void InspectorImageRenderer::BindPipeline(CommandList& cmd)
{
    auto* pso = GetPipeline(m_pipelineState);
    if (m_currentPSO == pso)
        return;

    cmd->SetPipelineState(pso);
    cmd->IASetPrimitiveTopology(ToD3D12_Draw(m_pipelineState.topologyType));
    m_currentPSO = pso;
}

void InspectorImageRenderer::Draw(CommandList& cmd, UINT srvIndex)
{
    InspectorTextureCB indices{};
    indices.srvIndex = srvIndex;
    auto cb = UploadDrawCB();

    cmd->SetGraphicsRoot32BitConstants(Core::ToIndex(RootSlot::ResourceIndices), 1, &indices, 0);
    cmd->SetGraphicsRootConstantBufferView(Core::ToIndex(RootSlot::DrawCB), cb);

    cmd->DrawInstanced(6, 1, 0, 0);
}

D3D12_GPU_VIRTUAL_ADDRESS InspectorImageRenderer::UploadDrawCB()
{
    InspectorDrawCB drawCB{};

    Core::Matrix world =
        Core::Matrix::Scale(ImageSize, ImageSize, 1.0f) *
        Core::Matrix::Translation(
            m_screenSize.width - ImageSize - Margin,
            m_screenSize.height - ImageSize - Margin,
            0.0f);

    DirectX::XMMATRIX xmWorld = ToDXMatrix(world);
    XMStoreFloat4x4(&drawCB.world, DirectX::XMMatrixTranspose(xmWorld));

    DirectX::XMMATRIX xmProj = ToDXMatrix(m_projection);
    XMStoreFloat4x4(&drawCB.projection, DirectX::XMMatrixTranspose(xmProj));

    drawCB.imageSize = { ImageSize, ImageSize }; 

    return m_drawCBAllocator.AllocateConstant(drawCB);
}

void InspectorImageRenderer::SetScreenSize(const Size& size)
{
    m_screenSize = size;

    m_projection = Core::Matrix::OrthographicOffCenter(
        0.0f,
        static_cast<float>(size.width),
        static_cast<float>(size.height),
        0.0f,
        0.0f,
        1.0f
    );
}