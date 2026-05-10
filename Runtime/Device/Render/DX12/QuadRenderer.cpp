#include "pch.h"
#include "QuadRenderer.h"
#include "CommandList.h"
#include "CommandUtils.h"
#include "DescriptorAllocation.h"
#include "MeshResource.h"
#include <d3dcompiler.h>

struct QuadTransform
{
    float scale[2];
    float offset[2];
};

using Microsoft::WRL::ComPtr;

QuadRenderer::~QuadRenderer() = default;
QuadRenderer::QuadRenderer() = default;

bool QuadRenderer::Initialize(ID3D12Device* device, const Size& screenSize)
{
    m_screenSize = screenSize;

    CD3DX12_DESCRIPTOR_RANGE rangeMesh;
    rangeMesh.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 0); // t0 vertex, t1 index

    CD3DX12_DESCRIPTOR_RANGE rangeTex;
    rangeTex.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2); //t2 texture

    CD3DX12_ROOT_PARAMETER params[3] = {};
    params[0].InitAsDescriptorTable(1, &rangeMesh);
    params[1].InitAsDescriptorTable(1, &rangeTex);
    params[2].InitAsConstantBufferView(0);

    CD3DX12_STATIC_SAMPLER_DESC sampler(
        0, // s0
        D3D12_FILTER_MIN_MAG_MIP_LINEAR
    );

    CD3DX12_ROOT_SIGNATURE_DESC rsDesc;
    rsDesc.Init(3, params, 1, &sampler,
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    ComPtr<ID3DBlob> sig{ nullptr };
    ComPtr<ID3DBlob> err{ nullptr };

    D3D12SerializeRootSignature(&rsDesc, D3D_ROOT_SIGNATURE_VERSION_1, &sig, &err);

    device->CreateRootSignature(
        0,
        sig->GetBufferPointer(),
        sig->GetBufferSize(),
        IID_PPV_ARGS(&m_rootSignature)
    );

    ComPtr<ID3DBlob> vs{ nullptr };
    ComPtr<ID3DBlob> ps{ nullptr };

    HRESULT hr = S_OK;
    wstring shaderFile = L"D:\\ProgrammingStudy\\ToyBox\\Runtime\\Device\\Render\\DX12\\Quad.hlsl";
    hr = D3DCompileFromFile(shaderFile.c_str(), nullptr, nullptr, "VSMain", "vs_5_0", 0, 0, &vs, &err);
    if (FAILED(hr)) return false;

    hr = D3DCompileFromFile(shaderFile.c_str(), nullptr, nullptr, "PSMain", "ps_5_0", 0, 0, &ps, &err);
    if (FAILED(hr)) return false;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
    psoDesc.InputLayout = { nullptr, 0 };
    psoDesc.pRootSignature = m_rootSignature.Get();
    psoDesc.VS = { vs->GetBufferPointer(), vs->GetBufferSize() };
    psoDesc.PS = { ps->GetBufferPointer(), ps->GetBufferSize() };

    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

    psoDesc.DepthStencilState.DepthEnable = FALSE;
    psoDesc.DepthStencilState.StencilEnable = FALSE;

    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    psoDesc.SampleDesc.Count = 1;

    hr = device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState));
    if (FAILED(hr)) return false;

    // Constant Buffer 생성
    CD3DX12_HEAP_PROPERTIES heap(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(256); // 256 align 필수

    HRESULT hrCB = device->CreateCommittedResource(
        &heap,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_constantBuffer)
    );
    if (FAILED(hrCB)) return false;

    // Map은 한 번만 (평생 유지)
    m_constantBuffer->Map(0, nullptr, (void**)&m_cbvData);

    return true;
}

void QuadRenderer::SetUIQuadMesh(std::shared_ptr<MeshResource> mesh)
{
    m_uiQuadMesh = std::move(mesh);
}

void QuadRenderer::BindPipeline(CommandList& cmd)
{
    cmd->SetGraphicsRootSignature(m_rootSignature.Get());
    cmd->SetPipelineState(m_pipelineState.Get());
}

void QuadRenderer::BindDescriptorHeap(CommandList& cmd)
{
    ID3D12DescriptorHeap* heaps[] = { m_srvHeap };
    cmd->SetDescriptorHeaps(1, heaps);
}

void QuadRenderer::BindTexture(CommandList& cmd, DescriptorAllocation& srv)
{
    auto handle = srv.GetGpuHandle();
    cmd->SetGraphicsRootDescriptorTable(1, handle);
    srv.MarkUsed(cmd.GetType(), cmd.GetFence());
}

void QuadRenderer::Draw(CommandList& cmd, const Rect& dest)
{
    if (!m_uiQuadMesh->IsReady()) 
        return;

    float left = (dest.Left() / (float)m_screenSize.width) * 2.0f - 1.0f;
    float right = (dest.Right() / (float)m_screenSize.width) * 2.0f - 1.0f;
    float top = 1.0f - (dest.Top() / (float)m_screenSize.height) * 2.0f;
    float bottom = 1.0f - (dest.Bottom() / (float)m_screenSize.height) * 2.0f;

    float width = right - left;
    float height = top - bottom;

    // scale / offset 계산
    m_cbvData->scale[0] = width;
    m_cbvData->scale[1] = height;
    m_cbvData->offset[0] = (left + right) * 0.5f;
    m_cbvData->offset[1] = (top + bottom) * 0.5f;

    auto& vbSrv = m_uiQuadMesh->GetVBSrv();
    auto& ibSrv = m_uiQuadMesh->GetIBSrv();

    cmd->SetGraphicsRootDescriptorTable(0, vbSrv.GetGpuHandle());
    cmd->SetGraphicsRootConstantBufferView(2, m_constantBuffer->GetGPUVirtualAddress());
    cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmd->DrawInstanced(m_uiQuadMesh->GetIndexCount(), 1, 0, 0);

    vbSrv.MarkUsed(cmd.GetType(), cmd.GetFence());
    ibSrv.MarkUsed(cmd.GetType(), cmd.GetFence());
}

void QuadRenderer::SetSRVHeap(ID3D12DescriptorHeap* heap)
{
    m_srvHeap = heap;
}