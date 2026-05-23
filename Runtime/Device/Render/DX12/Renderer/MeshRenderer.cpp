#include "pch.h"
#include "MeshRenderer.h"
#include "../MeshResource.h"
#include "../MeshMaterialResource.h"
#include "RootSignatureBuilder.h"
#include "../d3dx12.h"
#include <d3dcompiler.h>
#include "../Command/CommandList.h"
#include "../Descriptor/DescriptorAllocation.h"
#include "../ShaderSystem.h"
#include "../Helpers/MathHelpers.h"
#include "GameClient/Graphics/RenderData/DirectionalLightData.h"
#include "GameClient/Graphics/RenderData/CameraData.h"

namespace cm = Core::Math;

struct ObjectCB
{
    DirectX::XMFLOAT4X4 world;
};

struct FrameCB
{
    DirectX::XMFLOAT4X4 view;
    DirectX::XMFLOAT4X4 proj;

    float lightDirection[3];
    float lightIntensity;

    float lightColor[3];
    float padding;
};

struct MaterialCB
{
    float roughness;
    float metallic;

    float padding[2];
};

MeshRenderer::~MeshRenderer() = default;
MeshRenderer::MeshRenderer(ID3D12Device* device, ShaderSystem* shaderSystem) :
    m_device{ device },
    m_shaderSystem{ shaderSystem }
{}

bool MeshRenderer::Initialize()
{
    m_pipelineCache.Initialize(m_device, m_shaderSystem);

    ReturnIfFalse(CreateRootSignature());
    CreateDefaultPSOs();
    CreateConstantBuffers();

    return true;
}

void MeshRenderer::CreateDefaultPSOs()
{
    CreatePSO(PipelineLibrary::Get(ShaderID::Mesh, RasterPreset::Default));
    CreatePSO(PipelineLibrary::Get(ShaderID::Mesh, RasterPreset::NoCull));
    CreatePSO(PipelineLibrary::Get(ShaderID::Mesh, RasterPreset::Wireframe));
    CreatePSO(PipelineLibrary::Get(ShaderID::Mesh, RasterPreset::WireframeNoCull));
    CreatePSO(PipelineLibrary::Get(ShaderID::Mesh, RasterPreset::Default, PrimitiveTopologyType::Line));
}

ID3D12PipelineState* MeshRenderer::CreatePSO(const PipelineState& pipelineState)
{
    return m_pipelineCache.GetOrCreate(
        pipelineState,
        m_rootSignature.Get(),
        [&](D3D12_GRAPHICS_PIPELINE_STATE_DESC& pso)
        {
            pso.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
            pso.DepthStencilState.DepthEnable = TRUE;
            pso.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
            pso.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
            pso.DSVFormat = DXGI_FORMAT_D32_FLOAT;
        });
}

ID3D12PipelineState* MeshRenderer::GetPipeline(const PipelineState& pipelineState)
{
    auto* pipeline = m_pipelineCache.Find(pipelineState);
    if (pipeline)
        return pipeline;

    return CreatePSO(pipelineState);
}

bool MeshRenderer::CreateRootSignature()
{
    RootSignatureBuilder builder;

    builder.AddSRVTable(2, 0); // t0-t1
    builder.AddSRVTable(1, 2); // t2

    builder.AddCBV(0);
    builder.AddCBV(1);
    builder.AddCBV(2);
    
    builder.AddLinearSampler(0);

    m_rootSignature = builder.Build(m_device);
    return m_rootSignature != nullptr;
}

void MeshRenderer::CreateConstantBuffers()
{
    constexpr UINT objectBufferSize = kMaxObjectCount * kCBSize;
    constexpr UINT materialBufferSize = kMaxObjectCount * kCBSize;
    constexpr UINT frameBufferSize = kCBSize;

    m_objectCBAllocator.Initialize(m_device, objectBufferSize);
    m_materialCBAllocator.Initialize(m_device, materialBufferSize);
    m_frameCBAllocator.Initialize(m_device, frameBufferSize);
}

void MeshRenderer::BindCommonState(CommandList& cmd)
{
    cmd->SetGraphicsRootSignature(m_rootSignature.Get());

    ID3D12DescriptorHeap* heaps[] = { m_srvHeap };
    cmd->SetDescriptorHeaps(1, heaps);
}

void MeshRenderer::BindPipeline(CommandList& cmd, const PipelineState& pipelineState)
{
    cmd->SetPipelineState(GetPipeline(pipelineState));
    m_pipelineState = pipelineState;
}

void MeshRenderer::PrepareFrame(const DirectionalLightData& light, const CameraData& camera)
{
    m_objectCBAllocator.Reset();
    m_materialCBAllocator.Reset();
    m_frameCBAllocator.Reset();

    FrameCB frame{};
    DirectX::XMMATRIX view = ToDXMatrix(camera.view);
    DirectX::XMMATRIX proj = ToDXMatrix(camera.proj);

    // GPU용으로 transpose해서 저장
    XMStoreFloat4x4(&frame.view, DirectX::XMMatrixTranspose(view));
    XMStoreFloat4x4(&frame.proj, DirectX::XMMatrixTranspose(proj));

    // Directional Light
    frame.lightDirection[0] = light.direction.x;
    frame.lightDirection[1] = light.direction.y;
    frame.lightDirection[2] = light.direction.z;

    frame.lightColor[0] = light.color.x;
    frame.lightColor[1] = light.color.y;
    frame.lightColor[2] = light.color.z;

    frame.lightIntensity = light.intensity;

    m_frameCBAddress = m_frameCBAllocator.AllocateConstant(frame);
}

void MeshRenderer::Draw(
    CommandList& cmd, 
    MeshResource& mesh, 
    MeshMaterialResource& material,
    const cm::Matrix& world)
{
    auto objectCBAddress = UpdateObjectCB(world);
    auto materialCBAddress = UpdateMaterialCB(material.GetSurface());
    auto& meshTable = mesh.GetMeshTable();
    auto& textureSrv = material.GetAlbedoTextureSRV();
    
    cmd->SetGraphicsRootDescriptorTable(0, meshTable.GetGpuHandle());
    cmd->SetGraphicsRootDescriptorTable(1, textureSrv.GetGpuHandle());

    cmd->SetGraphicsRootConstantBufferView(2, objectCBAddress);
    cmd->SetGraphicsRootConstantBufferView(3, m_frameCBAddress);
    cmd->SetGraphicsRootConstantBufferView(4, materialCBAddress);

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

D3D12_GPU_VIRTUAL_ADDRESS MeshRenderer::UpdateObjectCB(const cm::Matrix& world)
{
    ObjectCB obj{};

    DirectX::XMMATRIX xmWorld = ToDXMatrix(world);
    XMStoreFloat4x4(&obj.world, DirectX::XMMatrixTranspose(xmWorld));

    return m_objectCBAllocator.AllocateConstant(obj);
}

D3D12_GPU_VIRTUAL_ADDRESS MeshRenderer::UpdateMaterialCB(const MaterialSurface& surface)
{
    MaterialCB cb{};

    cb.roughness = surface.roughness;
    cb.metallic = surface.metallic;

    return m_materialCBAllocator.AllocateConstant(cb);
}