#include "pch.h"
#include "UIRenderer.h"
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

UIRenderer::~UIRenderer() = default;
UIRenderer::UIRenderer(ID3D12Device* device, ShaderSystem* shaderSystem) :
    m_device{ device },
    m_shaderSystem{ shaderSystem }
{}

bool UIRenderer::Initialize(const Size& screenSize)
{
    m_pipelineCache.Initialize(m_device, m_shaderSystem);

    CreateRootSignature();
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