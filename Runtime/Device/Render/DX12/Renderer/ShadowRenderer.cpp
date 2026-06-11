#include "pch.h"
#include "ShadowRenderer.h"
#include "Asset/Mesh/MeshResource.h"
#include "RootSignatureBuilder.h"
#include "ShaderSystem.h"
#include "Command/CommandList.h"
#include "Helpers/MathHelpers.h"
#include "GameClient/Graphics/RenderData/DirectionalLightData.h"

namespace cm = Core::Math;

struct ShadowFrameCB
{
    DirectX::XMFLOAT4X4 lightViewProj;
};
CHECK_ALIGN16(ShadowFrameCB);

struct ShadowObjectCB
{
    DirectX::XMFLOAT4X4 world;
};
CHECK_ALIGN16(ShadowObjectCB);

ShadowRenderer::~ShadowRenderer() = default;
ShadowRenderer::ShadowRenderer(ID3D12Device* device, ShaderSystem* shaderSystem) :
    m_device{ device },
    m_shaderSystem{ shaderSystem }
{}

bool ShadowRenderer::Initialize()
{
    m_pipelineCache.Initialize(m_device, m_shaderSystem);

    ReturnIfFalse(CreateRootSignature());
    CreateDefaultPSOs();
    CreateConstantBuffers();

    return true;
}

void ShadowRenderer::CreateConstantBuffers()
{
    constexpr UINT objectBufferSize = kMaxObjectCount * kCBSize;
    constexpr UINT frameBufferSize = kCBSize;
    m_objectCBAllocator.Initialize(m_device, objectBufferSize);
    m_frameCBAllocator.Initialize(m_device, frameBufferSize);
}

bool ShadowRenderer::CreateRootSignature()
{
    RootSignatureBuilder builder;

    builder.Add32BitConstants(0, 2);
    builder.AddCBV(1); // b1 : objectCB
    builder.AddCBV(2); // b2 : shadowFrameCB (Light VP)

    m_rootSignature = builder.Build(m_device);
    return m_rootSignature != nullptr;
}

void ShadowRenderer::CreateDefaultPSOs()
{
    CreatePSO(PipelineLibrary::Get(ShadingModel::Shadow, RasterPreset::Default));
}

ID3D12PipelineState* ShadowRenderer::CreatePSO(const PipelineState& pipelineState)
{
    return m_pipelineCache.GetOrCreate(
        pipelineState,
        m_rootSignature.Get(),
        [&](D3D12_GRAPHICS_PIPELINE_STATE_DESC& pso)
        {
            pso.NumRenderTargets = 0;
            pso.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
            pso.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

            pso.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
            pso.DepthStencilState.DepthEnable = TRUE;
            pso.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
            pso.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
            pso.DSVFormat = DXGI_FORMAT_D32_FLOAT;

            pso.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
            pso.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;

            // 아티팩트 방지를 위한 하드웨어 뎁스 바이어스 설정 (수치는 상황에 따라 미세조정 필요)
            pso.RasterizerState.DepthBias = 3000;
            pso.RasterizerState.DepthBiasClamp = 0.0f;
            pso.RasterizerState.SlopeScaledDepthBias = 1.0f;
                            
            pso.PS = CD3DX12_SHADER_BYTECODE(nullptr, 0);
        });
}

ID3D12PipelineState* ShadowRenderer::GetPipeline(const PipelineState& pipelineState)
{
    auto* pipeline = m_pipelineCache.Find(pipelineState);
    if (pipeline)
        return pipeline;

    return CreatePSO(pipelineState);
}

void ShadowRenderer::BindCommonState(CommandList& cmd)
{
    ID3D12DescriptorHeap* heaps[] = { m_srvHeap };
    cmd->SetDescriptorHeaps(_countof(heaps), heaps);

    cmd->SetGraphicsRootSignature(m_rootSignature.Get());
}

void ShadowRenderer::BindPipeline(CommandList& cmd)
{
    PipelineState targetState = PipelineLibrary::Get(ShadingModel::Shadow, RasterPreset::Default);
    auto pipeline = GetPipeline(targetState);

    cmd->SetPipelineState(pipeline);
}

void ShadowRenderer::PrepareFrame(const DirectionalLightData& light)
{
    m_pipelineState = std::nullopt;

    m_objectCBAllocator.Reset();
    m_frameCBAllocator.Reset();

    ShadowFrameCB shadowFrame{};

    // 조명 데이터(light) 내부에 계산되어 저장되어 있을 Light View-Projection 행렬을 가져옴
    // 만약 Matrix 형식이 아니라면 프로젝트 변환 헬퍼(ToDXMatrix) 등을 거쳐 처리해줘
    DirectX::XMMATRIX lightVP = ToDXMatrix(light.viewProj);
    XMStoreFloat4x4(&shadowFrame.lightViewProj, DirectX::XMMatrixTranspose(lightVP));

    m_frameCBAddress = m_frameCBAllocator.AllocateConstant(shadowFrame);
}

D3D12_GPU_VIRTUAL_ADDRESS ShadowRenderer::UpdateObjectCB(const cm::Matrix& world)
{
    ShadowObjectCB obj{};
    DirectX::XMMATRIX xmWorld = ToDXMatrix(world);
    XMStoreFloat4x4(&obj.world, DirectX::XMMatrixTranspose(xmWorld));

    return m_objectCBAllocator.AllocateConstant(obj);
}

void ShadowRenderer::Draw(
    CommandList& cmd,
    MeshResource& mesh,
    const cm::Matrix& world)
{
    auto objectCBAddress = UpdateObjectCB(world);
    uint32_t meshIndices[2] = { mesh.GetVertexHeapIndex(), mesh.GetIndexHeapIndex() };

    cmd->SetGraphicsRoot32BitConstants(0, 2, meshIndices, 0);
    cmd->SetGraphicsRootConstantBufferView(1, objectCBAddress);
    cmd->SetGraphicsRootConstantBufferView(2, m_frameCBAddress);

    // 섀도우 맵은 항상 삼각형 리스트로 빌드업
    cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmd->DrawInstanced(mesh.GetIndexCount(), 1, 0, 0);
}