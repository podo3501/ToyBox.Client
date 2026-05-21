#include "pch.h"
#include "MeshRenderer.h"
#include "MeshResource.h"
#include "MeshMaterialResource.h"
#include "d3dx12.h"
#include <d3dcompiler.h>
#include "CommandList.h"
#include "DescriptorAllocation.h"
#include "ShaderSystem.h"
#include "DX12MathUtils.h"
#include "GameClient/Graphics/RenderData/DirectionalLightData.h"
#include "GameClient/Graphics/RenderData/CameraData.h"

namespace cm = Core::Math;

struct ObjectCB
{
    float world[16];
};

struct FrameCB
{
    float view[16];
    float proj[16];

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

bool MeshRenderer::Initialize(const Size& size)
{
    CreateRootSignature();
    CreateDefaultPSOs();
    CreateConstantBuffers();

    return true;
}

void MeshRenderer::CreateDefaultPSOs()
{
    CreatePipeline(PipelineLibrary::Get(ShaderID::Mesh, RasterPreset::Default));
    CreatePipeline(PipelineLibrary::Get(ShaderID::Mesh, RasterPreset::NoCull));
    CreatePipeline(PipelineLibrary::Get(ShaderID::Mesh, RasterPreset::Wireframe));
    CreatePipeline(PipelineLibrary::Get(ShaderID::Mesh, RasterPreset::WireframeNoCull));
    CreatePipeline(PipelineLibrary::Get(ShaderID::Mesh, RasterPreset::Default, PrimitiveTopologyType::Line));
}

ID3D12PipelineState* MeshRenderer::GetPipeline(const PipelineState& pipelineState)
{
    auto it = m_psoCache.find(pipelineState);
    if (it != m_psoCache.end())
        return it->second.Get();

    CreatePipeline(pipelineState);

    return m_psoCache[pipelineState].Get();
}

void MeshRenderer::CreatePipeline(const PipelineState& pipelineState)
{
    const ShaderEntry* shaderEntry = m_shaderSystem->Find(pipelineState.shaderVariant);
    if (!shaderEntry)
        return;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC pso{};
    pso.InputLayout = { nullptr, 0 };
    pso.pRootSignature = m_rootSignature.Get();

    pso.VS = { shaderEntry->vs->GetBufferPointer(), shaderEntry->vs->GetBufferSize() };
    pso.PS = { shaderEntry->ps->GetBufferPointer(), shaderEntry->ps->GetBufferSize() };

    //pso.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    CD3DX12_RASTERIZER_DESC raster(D3D12_DEFAULT);

    raster.FillMode =
        pipelineState.rasterState.fillMode == FillMode::Wireframe
        ? D3D12_FILL_MODE_WIREFRAME
        : D3D12_FILL_MODE_SOLID;

    switch (pipelineState.rasterState.cullMode)
    {
    case CullMode::None: raster.CullMode = D3D12_CULL_MODE_NONE; break;
    case CullMode::Front: raster.CullMode = D3D12_CULL_MODE_FRONT; break;
    case CullMode::Back: raster.CullMode = D3D12_CULL_MODE_BACK; break;
    }

    pso.RasterizerState = raster;
    pso.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

    pso.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    pso.DepthStencilState.DepthEnable = TRUE;
    pso.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    pso.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;

    pso.SampleMask = UINT_MAX;
    switch (pipelineState.topologyType)
    {
    case PrimitiveTopologyType::Triangle: pso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; break;
    case PrimitiveTopologyType::Line: pso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE; break;
    }
    pso.NumRenderTargets = 1;
    pso.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

    //pso.DSVFormat = DXGI_FORMAT_UNKNOWN;
    pso.DSVFormat = DXGI_FORMAT_D32_FLOAT;

    pso.SampleDesc.Count = 1;

    ComPtr<ID3D12PipelineState> pipeline;
    m_device->CreateGraphicsPipelineState(&pso, IID_PPV_ARGS(&pipeline));

    m_psoCache[pipelineState] = pipeline;
}

void MeshRenderer::CreateRootSignature()
{
    CD3DX12_DESCRIPTOR_RANGE rangeMesh;
    rangeMesh.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 0); // vb, ib

    CD3DX12_DESCRIPTOR_RANGE rangeTex;
    rangeTex.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2); // vb, ib

    CD3DX12_ROOT_PARAMETER params[5];
    params[0].InitAsDescriptorTable(1, &rangeMesh); // Mesh Table (t0-t1)
    params[1].InitAsDescriptorTable(1, &rangeTex);  // Texture Table (t2)
    params[2].InitAsConstantBufferView(0);      // object
    params[3].InitAsConstantBufferView(1);      // frame
    params[4].InitAsConstantBufferView(2);      // materialCB

    CD3DX12_STATIC_SAMPLER_DESC sampler(
        0,
        D3D12_FILTER_MIN_MAG_MIP_LINEAR
    );

    CD3DX12_ROOT_SIGNATURE_DESC desc;
    desc.Init(
        _countof(params),
        params,
        1,
        &sampler,
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
    );

    ComPtr<ID3DBlob> sig;
    ComPtr<ID3DBlob> err;

    D3D12SerializeRootSignature(
        &desc,
        D3D_ROOT_SIGNATURE_VERSION_1,
        &sig,
        &err
    );

    m_device->CreateRootSignature(
        0,
        sig->GetBufferPointer(),
        sig->GetBufferSize(),
        IID_PPV_ARGS(&m_rootSignature)
    );
}

void MeshRenderer::CreateConstantBuffers()
{
    CD3DX12_HEAP_PROPERTIES heap(D3D12_HEAP_TYPE_UPLOAD);
    auto cbDesc = CD3DX12_RESOURCE_DESC::Buffer(256);

    // Object CB
    for (uint32_t i = 0; i < kMaxObjectCount; ++i)
    {
        m_device->CreateCommittedResource(
            &heap,
            D3D12_HEAP_FLAG_NONE,
            &cbDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_objectCBs[i])
        );

        m_objectCBs[i]->Map(0, nullptr, reinterpret_cast<void**>(&m_objectDatas[i]));
    }

    // Frame CB
    m_device->CreateCommittedResource(
        &heap,
        D3D12_HEAP_FLAG_NONE,
        &cbDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_frameCB)
    );

    m_frameCB->Map(0, nullptr, (void**)&m_frameData);

    // Material CB
    for (uint32_t i = 0; i < kMaxObjectCount; ++i)
    {
        m_device->CreateCommittedResource(
            &heap,
            D3D12_HEAP_FLAG_NONE,
            &cbDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_materialCBs[i])
        );

        m_materialCBs[i]->Map(0, nullptr, reinterpret_cast<void**>(&m_materialDatas[i]));
    }
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
    m_objectCBIndex = 0;
    m_materialCBIndex = 0;

    FrameCB frame{};
    DirectX::XMMATRIX view = ToDXMatrix(camera.view);
    DirectX::XMMATRIX proj = ToDXMatrix(camera.proj);

    // GPU용으로 transpose해서 저장
    XMStoreFloat4x4(
        reinterpret_cast<DirectX::XMFLOAT4X4*>(frame.view),
        DirectX::XMMatrixTranspose(view));

    XMStoreFloat4x4(
        reinterpret_cast<DirectX::XMFLOAT4X4*>(frame.proj),
        DirectX::XMMatrixTranspose(proj));

    // Directional Light
    frame.lightDirection[0] = light.direction.x;
    frame.lightDirection[1] = light.direction.y;
    frame.lightDirection[2] = light.direction.z;

    frame.lightColor[0] = light.color.x;
    frame.lightColor[1] = light.color.y;
    frame.lightColor[2] = light.color.z;

    frame.lightIntensity = light.intensity;

    *m_frameData = frame;
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
    cmd->SetGraphicsRootConstantBufferView(3, m_frameCB->GetGPUVirtualAddress());
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
    Assert(m_objectCBIndex < kMaxObjectCount);

    ObjectCB obj{};

    DirectX::XMMATRIX xmWorld = ToDXMatrix(world);
    DirectX::XMStoreFloat4x4(
        reinterpret_cast<DirectX::XMFLOAT4X4*>(obj.world),
        DirectX::XMMatrixTranspose(xmWorld));

    *m_objectDatas[m_objectCBIndex] = obj;
    auto gpuAddress = m_objectCBs[m_objectCBIndex]->GetGPUVirtualAddress();

    ++m_objectCBIndex;
    return gpuAddress;
}

D3D12_GPU_VIRTUAL_ADDRESS MeshRenderer::UpdateMaterialCB(const MaterialSurface& surface)
{
    Assert(m_materialCBIndex < kMaxObjectCount);

    MaterialCB cb{};

    cb.roughness = surface.roughness;
    cb.metallic = surface.metallic;

    *m_materialDatas[m_materialCBIndex] = cb;
    auto gpuAddress = m_materialCBs[m_materialCBIndex]->GetGPUVirtualAddress();

    ++m_materialCBIndex;
    return gpuAddress;
}