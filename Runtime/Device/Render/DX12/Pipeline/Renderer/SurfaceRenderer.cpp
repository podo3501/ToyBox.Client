#include "pch.h"
#include "SurfaceRenderer.h"
#include "PipelineCache.h"
#include "RenderConstants.h"
#include "RootSignatureBuilder.h"
#include "Command/CommandList.h"
#include "Helpers/MathHelpers.h"
#include "Resource/Mesh/MeshResource.h"
#include "Resource/Material/PhongMaterialResource.h"
#include "Resource/Material/PbrMaterialResource.h"
#include "Resource/Environment/EnvironmentResource.h"
#include "Resource/Texture/TextureCubeResource.h"
#include "GameClient/Graphics/RenderData/FrameData.h"
#include "Core/RenderFormat.h"
#include "Core/D3D12Conversions.h"

//xxxStrength: 0에서 1 사이.
//xxxScale: 0에서 무한대. 원본값을 스케일하는 것.
//xxxIntensity: 0에서 무한대. 대신 원본값.

struct PbrMaterialCB
{
    uint32_t albedoTextureIndex;
    uint32_t normalTextureIndex;
    uint32_t armTextureIndex;
    float normalScale;
    float roughnessScale;
    float metallicScale;
    float aoStrength;
    float padding{ 0.f };
};

struct PhongMaterialCB
{
    uint32_t albedoTextureIndex; 
    uint32_t normalTextureIndex;
    uint32_t dummyTextureIndex{ 0 };
    float normalScale;
    float ambientScale;
    float specularScale;
    float shininess; //하이라이트 지수: 보통 4.0 ~ 256.0
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
SurfaceRenderer::SurfaceRenderer(const SurfaceRendererConfig& config, PipelineCache& pipelineCache) :
    m_config{ config },
    m_pipelineCache{ pipelineCache }
{}

bool SurfaceRenderer::Initialize(Device& device)
{
    m_objectCBAllocator.Initialize<ObjectCB>(device, m_config.maxObjectCount);
    m_materialCBAllocator.Initialize<MaterialConstantBuffer>(device, m_config.maxObjectCount);
    m_frameCBAllocator.Initialize<MeshFrameCB>(device, 2);

    ReturnIfFalse(CreateRootSignature(device));
    ReturnIfFalse(CreateDefaultPSOs());

    return true;
}

bool SurfaceRenderer::CreateDefaultPSOs()
{
    ReturnIfFalse(CreatePSO(PipelineLibrary::Get(RegistryShader::Phong, RasterPreset::Default)) != nullptr);
    ReturnIfFalse(CreatePSO(PipelineLibrary::Get(RegistryShader::Phong, RasterPreset::NoCull)) != nullptr);
    ReturnIfFalse(CreatePSO(PipelineLibrary::Get(RegistryShader::Phong, RasterPreset::Wireframe)) != nullptr);
    ReturnIfFalse(CreatePSO(PipelineLibrary::Get(RegistryShader::Phong, RasterPreset::WireframeNoCull)) != nullptr);

    ReturnIfFalse(CreatePSO(PipelineLibrary::Get(RegistryShader::PBR, RasterPreset::Default)) != nullptr);

    return true;
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
            pso.DSVFormat = RenderFormat::DepthFormat;;
        });
}

ID3D12PipelineState* SurfaceRenderer::GetPipeline(const PipelineState& pipelineState)
{
    auto* pipeline = m_pipelineCache.Find(pipelineState);
    if (pipeline)
        return pipeline;

    return CreatePSO(pipelineState);
}

bool SurfaceRenderer::CreateRootSignature(Device& device)
{
    RootSignatureBuilder builder;

    builder.Add32BitConstants(Core::ToIndex(RootSlot::MeshData), 2);
    builder.AddCBV(Core::ToIndex(RootSlot::FrameCB));
    builder.AddCBV(Core::ToIndex(RootSlot::ObjectCB));
    builder.AddCBV(Core::ToIndex(RootSlot::MaterialCB));

    builder.AddFlags(D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
    
    builder.AddLinearSampler(0);
    builder.AddComparisonSampler(1);

    m_rootSignature = builder.Build(device);
    return m_rootSignature != nullptr;
}

void SurfaceRenderer::ResetFrameResources()
{
    m_objectCBAllocator.Reset();
    m_materialCBAllocator.Reset();
    m_frameCBAllocator.Reset();
}

D3D12_GPU_VIRTUAL_ADDRESS  SurfaceRenderer::PrepareFrame(
    const DirectionalLightData& light, 
    const CameraData& camera, 
    uint32_t shadowSRVIndex,
    const EnvironmentResource* envRes)
{
    MeshFrameCB meshFrame{};
    DirectX::XMMATRIX view = ToDXMatrix(camera.view);
    DirectX::XMMATRIX proj = ToDXMatrix(camera.proj);
    DirectX::XMMATRIX lightVP = ToDXMatrix(light.viewProj);

    // GPU용으로 transpose해서 저장
    XMStoreFloat4x4(&meshFrame.view, DirectX::XMMatrixTranspose(view));
    XMStoreFloat4x4(&meshFrame.proj, DirectX::XMMatrixTranspose(proj));
    XMStoreFloat4x4(&meshFrame.lightViewProj, DirectX::XMMatrixTranspose(lightVP));

    meshFrame.cameraPosition = ToXMFLOAT3(camera.position);
    meshFrame.lightDirection = ToXMFLOAT3(light.direction);
    meshFrame.lightIntensity = light.intensity;
    meshFrame.lightColor = ToXMFLOAT3(light.color);
    meshFrame.shadowTextureIndex = shadowSRVIndex;

    if (envRes && envRes->IsReady())
    {
        meshFrame.reflectionTextureIndex = envRes->GetReflection()->GetHeapIndex();
        meshFrame.reflectionMipCount = envRes->GetReflection()->GetMipCount();

        const auto& sh = envRes->GetIrradianceSH();
        for (int i = 0; i < 9; ++i)
            meshFrame.irradianceSH[i] = { sh[i].x, sh[i].y, sh[i].z, 0.0f };
    }
    else
    {
        meshFrame.reflectionTextureIndex = UINT_MAX;
        meshFrame.reflectionMipCount = 0;
        // irradianceSH는 {} 초기화라 이미 0
    }

    return m_frameCBAllocator.AllocateConstant(meshFrame);
}

void SurfaceRenderer::BeginFrame(CommandList& cmd, D3D12_GPU_VIRTUAL_ADDRESS frameCBAddress)
{
    m_currentPSO = nullptr;

    cmd->SetGraphicsRootSignature(m_rootSignature.Get());
    cmd->SetGraphicsRootConstantBufferView(Core::ToIndex(RootSlot::FrameCB), frameCBAddress);
}

void SurfaceRenderer::BindPipeline(CommandList& cmd, const PipelineState& pipelineState)
{
    auto* pso = GetPipeline(pipelineState);
    if (m_currentPSO == pso)
        return;

    cmd->SetPipelineState(pso);
    cmd->IASetPrimitiveTopology(ToD3D12_Draw(pipelineState.topologyType));
    m_currentPSO = pso;
}

D3D12_GPU_VIRTUAL_ADDRESS SurfaceRenderer::UploadObjectCB(const Core::Matrix& world)
{
    ObjectCB obj{};

    DirectX::XMMATRIX xmWorld = ToDXMatrix(world);
    XMStoreFloat4x4(&obj.world, DirectX::XMMatrixTranspose(xmWorld));

    return m_objectCBAllocator.AllocateConstant(obj);
}

D3D12_GPU_VIRTUAL_ADDRESS SurfaceRenderer::UploadMaterialCB(MaterialResource& material)
{
    MaterialConstantBuffer gpuCB{};

    switch (material.GetType())
    {
    case MaterialType::Phong:
    {
        auto& phongMat = static_cast<PhongMaterialResource&>(material);
        const PhongSurface& surface = phongMat.GetSurface();

        PhongMaterialCB cb{};
        cb.albedoTextureIndex = phongMat.GetAlbedo().GetHeapIndex();
        cb.normalTextureIndex = phongMat.GetNormal().GetHeapIndex();
        cb.dummyTextureIndex = 0;

        cb.normalScale = surface.normalScale;
        cb.ambientScale = surface.ambientScale;
        cb.specularScale = surface.specularScale;
        cb.shininess = surface.shininess;

        std::memcpy(&gpuCB, &cb, sizeof(MaterialConstantBuffer));
        break;
    }
    case MaterialType::PBR:
    {
        auto& pbrMat = static_cast<PbrMaterialResource&>(material);
        const PbrSurface& surface = pbrMat.GetSurface();

        PbrMaterialCB cb{};
        cb.albedoTextureIndex = pbrMat.GetAlbedo().GetHeapIndex();
        cb.normalTextureIndex = pbrMat.GetNormal().GetHeapIndex();
        cb.armTextureIndex = pbrMat.GetArm().GetHeapIndex();
             
        cb.normalScale = surface.normalScale;
        cb.roughnessScale = surface.roughnessScale;
        cb.metallicScale = surface.metallicScale;
        cb.aoStrength = surface.aoStrength;

        std::memcpy(&gpuCB, &cb, sizeof(MaterialConstantBuffer));
        break;
    }
    }

    return m_materialCBAllocator.AllocateConstant(gpuCB);
}

void SurfaceRenderer::Draw(
    CommandList& cmd,
    MeshResource& mesh,
    MaterialResource& material,
    const Core::Matrix& world)
{
    auto objectCBAddress = UploadObjectCB(world);
    auto materialCBAddress = UploadMaterialCB(material);
    uint32_t meshIndices[2] = { mesh.GetVertexHeapIndex(), mesh.GetIndexHeapIndex() };

    cmd->SetGraphicsRoot32BitConstants(Core::ToIndex(RootSlot::MeshData), 2, meshIndices, 0);
    cmd->SetGraphicsRootConstantBufferView(Core::ToIndex(RootSlot::ObjectCB), objectCBAddress);
    cmd->SetGraphicsRootConstantBufferView(Core::ToIndex(RootSlot::MaterialCB), materialCBAddress);

    cmd->DrawInstanced(mesh.GetIndexCount(), 1, 0, 0);
}
