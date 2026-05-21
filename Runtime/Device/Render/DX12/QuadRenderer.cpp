#include "pch.h"
#include "QuadRenderer.h"
#include "CommandList.h"
#include "CommandUtils.h"
#include "DescriptorAllocation.h"
#include "MeshResource.h"
#include "UIMaterialResource.h"
#include "ShaderSystem.h"
#include "DX12MathUtils.h"
#include <d3dcompiler.h>

namespace cm = Core::Math;

struct UIFrameCB
{
    DirectX::XMFLOAT4X4 world;
    DirectX::XMFLOAT4X4 projection;
};

using Microsoft::WRL::ComPtr;

QuadRenderer::~QuadRenderer() = default;
QuadRenderer::QuadRenderer(ID3D12Device* device, ShaderSystem* shaderSystem) :
    m_device{ device },
    m_shaderSystem{ shaderSystem }
{}

bool QuadRenderer::Initialize(const Size& screenSize)
{
    CreateRootSignature();
    CreateDefaultPSOs();
    CreateConstantBuffers();
    SetScreenSize(screenSize);

    return true;
}

void QuadRenderer::CreateDefaultPSOs()
{
    CreatePipeline(PipelineLibrary::Get(ShaderID::UI, RasterPreset::NoCull));
}

ID3D12PipelineState* QuadRenderer::GetPipeline(const PipelineState& pipelineState)
{
    auto it = m_psoCache.find(pipelineState);
    if (it != m_psoCache.end())
        return it->second.Get();

    CreatePipeline(pipelineState);

    return m_psoCache[pipelineState].Get();
}

bool QuadRenderer::CreateRootSignature()
{
    CD3DX12_DESCRIPTOR_RANGE rangeMesh;
    rangeMesh.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 0); // t0 vertex, t1 index

    CD3DX12_DESCRIPTOR_RANGE rangeTex;
    rangeTex.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2); //t2 texture

    CD3DX12_ROOT_PARAMETER params[3] = {};
    params[0].InitAsDescriptorTable(1, &rangeMesh);
    params[1].InitAsDescriptorTable(1, &rangeTex);
    params[2].InitAsConstantBufferView(1); //b1

    CD3DX12_STATIC_SAMPLER_DESC sampler(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR); // s0

    CD3DX12_ROOT_SIGNATURE_DESC rsDesc;
    rsDesc.Init(_countof(params), params, 1, &sampler, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    ComPtr<ID3DBlob> sig{ nullptr };
    ComPtr<ID3DBlob> err{ nullptr };

    HRESULT hr = D3D12SerializeRootSignature(&rsDesc, D3D_ROOT_SIGNATURE_VERSION_1, &sig, &err);
    if (FAILED(hr))
    {
        if (err)
            OutputDebugStringA(static_cast<const char*>(err->GetBufferPointer()));
        return false;
    }

    m_device->CreateRootSignature(
        0,
        sig->GetBufferPointer(),
        sig->GetBufferSize(),
        IID_PPV_ARGS(&m_rootSignature)
    );

    return true;
}

void QuadRenderer::CreatePipeline(const PipelineState& pipelineState)
{
    const ShaderEntry* shaderEntry = m_shaderSystem->Find(pipelineState.shaderVariant);
    if (!shaderEntry)
        return;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC pso{};
    pso.InputLayout = { nullptr, 0 };
    pso.pRootSignature = m_rootSignature.Get();

    pso.VS = { shaderEntry->vs->GetBufferPointer(), shaderEntry->vs->GetBufferSize() };
    pso.PS = { shaderEntry->ps->GetBufferPointer(), shaderEntry->ps->GetBufferSize() };

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
    pso.DepthStencilState.DepthEnable = FALSE;
    pso.DepthStencilState.StencilEnable = FALSE;

    pso.SampleMask = UINT_MAX;
    switch (pipelineState.topologyType)
    {
    case PrimitiveTopologyType::Triangle: pso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; break;
    case PrimitiveTopologyType::Line: pso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE; break;
    }
    pso.NumRenderTargets = 1;
    pso.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    pso.SampleDesc.Count = 1;

    ComPtr<ID3D12PipelineState> pipeline;
    m_device->CreateGraphicsPipelineState(&pso, IID_PPV_ARGS(&pipeline));
    m_psoCache[pipelineState] = pipeline;
}

void QuadRenderer::BindCommonState(CommandList& cmd)
{
    cmd->SetGraphicsRootSignature(m_rootSignature.Get());

    ID3D12DescriptorHeap* heaps[] = { m_srvHeap };
    cmd->SetDescriptorHeaps(1, heaps);
}

void QuadRenderer::BindPipeline(CommandList& cmd, const PipelineState& pipelineState)
{
    cmd->SetPipelineState(GetPipeline(pipelineState));
    m_pipelineState = pipelineState;
}

void QuadRenderer::PrepareFrame()
{
    m_uiCount = 0;
}

void QuadRenderer::Draw(
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
    
    //*m_uiFrameData = frameCB;

    //cmd->SetGraphicsRootConstantBufferView(2, m_uiFrameCB->GetGPUVirtualAddress());

    UINT offset = m_uiCount * kCBSize;

    memcpy(
        reinterpret_cast<uint8_t*>(m_uiFrameData) + offset,
        &frameCB,
        sizeof(UIFrameCB)
    );

    cmd->SetGraphicsRootConstantBufferView(
        2,
        m_uiFrameCB->GetGPUVirtualAddress() + offset
    );

    m_uiCount++;

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

void QuadRenderer::SetScreenSize(const Size& size)
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

void QuadRenderer::CreateConstantBuffers()
{
    UINT bufferSize = kMaxUI * kCBSize;

    CD3DX12_HEAP_PROPERTIES heap(D3D12_HEAP_TYPE_UPLOAD);
    auto cbDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

    m_device->CreateCommittedResource(
        &heap,
        D3D12_HEAP_FLAG_NONE,
        &cbDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_uiFrameCB)
    );

    m_uiFrameCB->Map(0, nullptr, reinterpret_cast<void**>(&m_uiFrameData));
}