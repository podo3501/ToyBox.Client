#include "pch.h"
#include "SurfaceRenderer.h"
#include "RenderConstants.h"
#include "RootSignatureBuilder.h"
#include "Command/CommandList.h"
#include "Helpers/MathHelpers.h"
#include "Resource/Mesh/MeshResource.h"
#include "Resource/Material/PhongMaterialResource.h"
#include "Resource/Material/PbrMaterialResource.h"
#include "Resource/Shader/ShaderProvider.h"
#include "GameClient/Graphics/RenderData/DirectionalLightData.h"
#include "GameClient/Graphics/RenderData/CameraData.h"
#include "d3dx12.h"
#include <d3dcompiler.h>

namespace cm = Core::Math;

struct PbrMaterialCB
{
    uint32_t albedoTextureIndex;
    uint32_t normalTextureIndex;
    uint32_t armTextureIndex;
    float normalIntensity;
    float roughnessIntensity;
    float ambientOcclusionIntensity;
    float metallic;
    float padding{ 0.f };
};

struct PhongMaterialCB
{
    uint32_t albedoTextureIndex; 
    uint32_t normalTextureIndex;
    uint32_t dummyTextureIndex{ 0 };
    float normalIntensity;
    float shininess;
    float specularIntensity;
    float ambientIntensity;
    float padding{ 0.f };
};

struct MaterialConstantBuffer
{
    uint32_t textureIndices[3]; //텍스쳐 인덱스
    float    param[5];
};
CHECK_ALIGN16(MaterialConstantBuffer);

static_assert(sizeof(PbrMaterialCB) == sizeof(MaterialConstantBuffer)); 
static_assert(sizeof(PhongMaterialCB) == sizeof(MaterialConstantBuffer));

SurfaceRenderer::~SurfaceRenderer() = default;
SurfaceRenderer::SurfaceRenderer(ID3D12Device* device, ShaderProvider* shaderProvider) :
    m_device{ device },
    m_shaderProvider{ shaderProvider }
{}

bool SurfaceRenderer::Initialize()
{
    m_pipelineCache.Initialize(m_device, m_shaderProvider);

    ReturnIfFalse(CreateRootSignature());
    CreateDefaultPSOs();
    CreateConstantBuffers();

    return true;
}

void SurfaceRenderer::CreateDefaultPSOs()
{
    CreatePSO(PipelineLibrary::Get(ShadingModel::Phong, RasterPreset::Default));
    CreatePSO(PipelineLibrary::Get(ShadingModel::Phong, RasterPreset::NoCull));
    CreatePSO(PipelineLibrary::Get(ShadingModel::Phong, RasterPreset::Wireframe));
    CreatePSO(PipelineLibrary::Get(ShadingModel::Phong, RasterPreset::WireframeNoCull));

    CreatePSO(PipelineLibrary::Get(ShadingModel::PBR, RasterPreset::Default));
}

ID3D12PipelineState* SurfaceRenderer::CreatePSO(const PipelineState& pipelineState)
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

ID3D12PipelineState* SurfaceRenderer::GetPipeline(const PipelineState& pipelineState)
{
    auto* pipeline = m_pipelineCache.Find(pipelineState);
    if (pipeline)
        return pipeline;

    return CreatePSO(pipelineState);
}

bool SurfaceRenderer::CreateRootSignature()
{
    RootSignatureBuilder builder;

    builder.Add32BitConstants(0, 2); // b0

    builder.AddCBV(1); //objectCB
    builder.AddCBV(2); //meshFrameCB
    builder.AddCBV(3); //materialCB
    
    builder.AddLinearSampler(0);
    builder.AddComparisonSampler(1);

    m_rootSignature = builder.Build(m_device);
    return m_rootSignature != nullptr;
}

void SurfaceRenderer::CreateConstantBuffers()
{
    constexpr UINT objectBufferSize = kMaxObjectCount * kCBSize;
    constexpr UINT materialBufferSize = kMaxObjectCount * kCBSize;
    constexpr UINT frameBufferSize = kCBSize;

    m_objectCBAllocator.Initialize(m_device, objectBufferSize);
    m_materialCBAllocator.Initialize(m_device, materialBufferSize);
    m_frameCBAllocator.Initialize(m_device, frameBufferSize);
}

void SurfaceRenderer::BindCommonState(CommandList& cmd)
{
    ID3D12DescriptorHeap* heaps[] = { m_srvHeap };
    cmd->SetDescriptorHeaps(1, heaps);

    cmd->SetGraphicsRootSignature(m_rootSignature.Get());
}

void SurfaceRenderer::BindPipeline(CommandList& cmd, const PipelineState& pipelineState)
{
    if (m_pipelineState && *m_pipelineState == pipelineState)
        return;

    auto pipeline = GetPipeline(pipelineState);
    cmd->SetPipelineState(pipeline);
    m_pipelineState = pipelineState;
}

void SurfaceRenderer::PrepareFrame(const DirectionalLightData& light, const CameraData& camera, uint32_t shadowSRVIndex)
{
    m_pipelineState = std::nullopt;

    m_objectCBAllocator.Reset();
    m_materialCBAllocator.Reset();
    m_frameCBAllocator.Reset();

    MeshFrameCB meshFrame{};
    DirectX::XMMATRIX view = ToDXMatrix(camera.view);
    DirectX::XMMATRIX proj = ToDXMatrix(camera.proj);
    DirectX::XMMATRIX lightVP = ToDXMatrix(light.viewProj);

    // GPU용으로 transpose해서 저장
    XMStoreFloat4x4(&meshFrame.view, DirectX::XMMatrixTranspose(view));
    XMStoreFloat4x4(&meshFrame.proj, DirectX::XMMatrixTranspose(proj));
    XMStoreFloat4x4(&meshFrame.lightViewProj, DirectX::XMMatrixTranspose(lightVP));

    // Camera Position
    meshFrame.cameraPosition[0] = camera.position.x;
    meshFrame.cameraPosition[1] = camera.position.y;
    meshFrame.cameraPosition[2] = camera.position.z;

    // Directional Light
    meshFrame.lightDirection[0] = light.direction.x;
    meshFrame.lightDirection[1] = light.direction.y;
    meshFrame.lightDirection[2] = light.direction.z;

    meshFrame.lightColor[0] = light.color.x;
    meshFrame.lightColor[1] = light.color.y;
    meshFrame.lightColor[2] = light.color.z;

    meshFrame.lightIntensity = light.intensity;
    meshFrame.shadowTextureIndex = shadowSRVIndex;

    m_frameCBAddress = m_frameCBAllocator.AllocateConstant(meshFrame);
}

D3D12_GPU_VIRTUAL_ADDRESS SurfaceRenderer::UpdateObjectCB(const cm::Matrix& world)
{
    ObjectCB obj{};

    DirectX::XMMATRIX xmWorld = ToDXMatrix(world);
    XMStoreFloat4x4(&obj.world, DirectX::XMMatrixTranspose(xmWorld));

    return m_objectCBAllocator.AllocateConstant(obj);
}

D3D12_GPU_VIRTUAL_ADDRESS SurfaceRenderer::UpdateMaterialCB(MaterialResource& material)
{
    SurfaceMaterialResource* surfaceMat = static_cast<SurfaceMaterialResource*>(&material);
    auto textureIndices = surfaceMat->GetTextureIndices();

    MaterialConstantBuffer gpuCB{};
    switch (surfaceMat->GetSurfaceType())
    {
    case SurfaceType::Phong:
    {
        auto* phongMat = static_cast<PhongMaterialResource*>(surfaceMat);
        const PhongSurface& surface = phongMat->GetSurface();

        PhongMaterialCB cb{};
        cb.albedoTextureIndex = textureIndices[static_cast<int>(PhongTextureSlot::Albedo)];
        cb.normalTextureIndex = textureIndices[static_cast<int>(PhongTextureSlot::Normal)];
        cb.dummyTextureIndex = 0;

        cb.normalIntensity = surface.normal;
        cb.shininess = surface.shininess;
        cb.specularIntensity = surface.specular;
        cb.ambientIntensity = surface.ambient;

        std::memcpy(&gpuCB, &cb, sizeof(MaterialConstantBuffer));
        break;
    }
    case SurfaceType::PBR:
    {
        auto* pbrMat = static_cast<PbrMaterialResource*>(surfaceMat);
        const PbrSurface& surface = pbrMat->GetSurface();

        PbrMaterialCB cb{};
        cb.albedoTextureIndex = textureIndices[static_cast<int>(PbrTextureSlot::Albedo)];
        cb.normalTextureIndex = textureIndices[static_cast<int>(PbrTextureSlot::Normal)];
        cb.armTextureIndex = textureIndices[static_cast<int>(PbrTextureSlot::ARM)];

        cb.normalIntensity = surface.normal;
        cb.roughnessIntensity = surface.roughness;
        cb.ambientOcclusionIntensity = surface.ao;
        cb.metallic = surface.metallic;

        std::memcpy(&gpuCB, &cb, sizeof(MaterialConstantBuffer));
        break;
    }
    default: Assert(false); break; // 미지원 또는 구현이 누락된 SurfaceType
    }

    return m_materialCBAllocator.AllocateConstant(gpuCB);
}

void SurfaceRenderer::Draw(
    CommandList& cmd,
    MeshResource& mesh,
    MaterialResource& material,
    const cm::Matrix& world)
{
    auto objectCBAddress = UpdateObjectCB(world);
    auto materialCBAddress = UpdateMaterialCB(material);
    uint32_t meshIndices[2] = { mesh.GetVertexHeapIndex(), mesh.GetIndexHeapIndex() };

    cmd->SetGraphicsRoot32BitConstants(0, 2, meshIndices, 0);
    cmd->SetGraphicsRootConstantBufferView(1, objectCBAddress);
    cmd->SetGraphicsRootConstantBufferView(2, m_frameCBAddress);
    cmd->SetGraphicsRootConstantBufferView(3, materialCBAddress);

    switch (m_pipelineState->topologyType)
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
