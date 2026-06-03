#include "pch.h"
#include "MeshRenderer.h"
#include "../MeshResource.h"
#include "../MeshMaterialResource.h"
#include "RenderConstants.h"
#include "RootSignatureBuilder.h"
#include "../d3dx12.h"
#include <d3dcompiler.h>
#include "../Command/CommandList.h"
#include "../ShaderSystem.h"
#include "../Helpers/MathHelpers.h"
#include "GameClient/Graphics/RenderData/DirectionalLightData.h"
#include "GameClient/Graphics/RenderData/CameraData.h"

namespace cm = Core::Math;

struct MaterialCB
{
    uint32_t albedoTextureIndex;
    uint32_t normalTextureIndex;
    float normalIntensity;
    float roughnessIntensity;
    float metallic;
    float matPadding[3];
};
CHECK_ALIGN16(MaterialCB);

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

    builder.Add32BitConstants(0, 2); // b0

    builder.AddCBV(1); //objectCB
    builder.AddCBV(2); //meshFrameCB
    builder.AddCBV(3); //materialCB
    
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
    ID3D12DescriptorHeap* heaps[] = { m_srvHeap };
    cmd->SetDescriptorHeaps(1, heaps);

    cmd->SetGraphicsRootSignature(m_rootSignature.Get());
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

    MeshFrameCB meshFrame{};
    DirectX::XMMATRIX view = ToDXMatrix(camera.view);
    DirectX::XMMATRIX proj = ToDXMatrix(camera.proj);

    // GPU용으로 transpose해서 저장
    XMStoreFloat4x4(&meshFrame.frame.view, DirectX::XMMatrixTranspose(view));
    XMStoreFloat4x4(&meshFrame.frame.proj, DirectX::XMMatrixTranspose(proj));

    // Directional Light
    meshFrame.lighting.lightDirection[0] = light.direction.x;
    meshFrame.lighting.lightDirection[1] = light.direction.y;
    meshFrame.lighting.lightDirection[2] = light.direction.z;

    meshFrame.lighting.lightColor[0] = light.color.x;
    meshFrame.lighting.lightColor[1] = light.color.y;
    meshFrame.lighting.lightColor[2] = light.color.z;

    meshFrame.lighting.lightIntensity = light.intensity;

    m_frameCBAddress = m_frameCBAllocator.AllocateConstant(meshFrame);
}

void MeshRenderer::Draw(
    CommandList& cmd, 
    MeshResource& mesh, 
    MeshMaterialResource& material,
    const cm::Matrix& world)
{
    auto objectCBAddress = UpdateObjectCB(world);
    auto materialCBAddress = UpdateMaterialCB(material.GetSurface(), material.GetTextureIndices());
    uint32_t meshIndices[2] = { mesh.GetVertexHeapIndex(), mesh.GetIndexHeapIndex() };
    
    cmd->SetGraphicsRoot32BitConstants(0, 2, meshIndices, 0);
    cmd->SetGraphicsRootConstantBufferView(1, objectCBAddress);
    cmd->SetGraphicsRootConstantBufferView(2, m_frameCBAddress);
    cmd->SetGraphicsRootConstantBufferView(3, materialCBAddress);

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
}

D3D12_GPU_VIRTUAL_ADDRESS MeshRenderer::UpdateObjectCB(const cm::Matrix& world)
{
    ObjectCB obj{};

    DirectX::XMMATRIX xmWorld = ToDXMatrix(world);
    XMStoreFloat4x4(&obj.world, DirectX::XMMatrixTranspose(xmWorld));

    return m_objectCBAllocator.AllocateConstant(obj);
}

D3D12_GPU_VIRTUAL_ADDRESS MeshRenderer::UpdateMaterialCB(
    const MaterialSurface& surface, std::vector<UINT> textureIndices)
{
    MaterialCB cb{};

    cb.albedoTextureIndex = textureIndices[static_cast<int>(MeshTextureSlot::Albedo)];
    cb.normalTextureIndex = textureIndices[static_cast<int>(MeshTextureSlot::Normal)];
    cb.normalIntensity = surface.normalIntensity;
    cb.roughnessIntensity = surface.roughnessIntensity;
    cb.metallic = surface.metallic;

    return m_materialCBAllocator.AllocateConstant(cb);
}

//텍스쳐가 뭔가 잘못 올라간거 같다. 왠지는 모르겠고.. 찾아봐야 할듯. 일단 hlsl 문제는 아닌듯.