#include "pch.h"
#include "QuadRenderer.h"
#include "d3dx12.h"
#include <d3dcompiler.h>

using Microsoft::WRL::ComPtr;

struct Vertex { float x, y, z; float r, g, b, a; };

QuadRenderer::~QuadRenderer() = default;
QuadRenderer::QuadRenderer(DX12Core core) : 
	m_core(core) 
{}

bool QuadRenderer::Initialize()
{
    Vertex quad[] =
    {
        { -0.5f, -0.5f, 0, 1, 0, 0, 1 },
        { -0.5f, 0.5f, 0, 0, 1, 0, 1 },
        { 0.5f, -0.5f, 0, 0, 0, 1, 1 },

        { 0.5f, -0.5f, 0, 0, 0, 1, 1 },
        { -0.5f, 0.5f, 0, 0, 1, 0, 1 },
        { 0.5f, 0.5f, 0, 1, 1, 1, 1 },
    };

    UINT vbSize = sizeof(quad);

    CD3DX12_HEAP_PROPERTIES heap(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(vbSize);

    m_core.device->CreateCommittedResource(&heap, D3D12_HEAP_FLAG_NONE, &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_vertexBuffer)); //VertexBuffer (Upload Heap)

    void* data;
    m_vertexBuffer->Map(0, nullptr, &data);
    memcpy(data, quad, vbSize);
    m_vertexBuffer->Unmap(0, nullptr);

    m_vertexView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
    m_vertexView.SizeInBytes = vbSize;
    m_vertexView.StrideInBytes = sizeof(Vertex);

    CD3DX12_ROOT_SIGNATURE_DESC rsDesc;
    rsDesc.Init(0, nullptr, 0, nullptr,
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT); // RootSignature(비어있음)

    ComPtr<ID3DBlob> sig{ nullptr };
    ComPtr<ID3DBlob> err{ nullptr };

    D3D12SerializeRootSignature(&rsDesc, D3D_ROOT_SIGNATURE_VERSION_1, &sig, &err);

    m_core.device->CreateRootSignature(
        0,
        sig->GetBufferPointer(),
        sig->GetBufferSize(),
        IID_PPV_ARGS(&m_rootSignature)
    );

    ComPtr<ID3DBlob> vs{ nullptr };
    ComPtr<ID3DBlob> ps{ nullptr };

    wstring shaderFile = L"D:\\ProgrammingStudy\\ToyBox\\Runtime\\Device\\Render\\DX12\\Quad.hlsl";
    D3DCompileFromFile(shaderFile.c_str(), nullptr, nullptr, "VSMain", "vs_5_0", 0, 0, &vs, nullptr);
    D3DCompileFromFile(shaderFile.c_str(), nullptr, nullptr, "PSMain", "ps_5_0", 0, 0, &ps, nullptr);

    D3D12_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },

        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    }; // Input Layout

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
    psoDesc.InputLayout = { layout, _countof(layout) };
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
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.SampleDesc.Count = 1;

    m_core.device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState));
    return true;
}

void QuadRenderer::BindPipeline(ID3D12GraphicsCommandList* cmd)
{
    cmd->SetGraphicsRootSignature(m_rootSignature.Get());
    cmd->SetPipelineState(m_pipelineState.Get());
}

void QuadRenderer::Draw(ID3D12GraphicsCommandList* cmd)
{
    cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmd->IASetVertexBuffers(0, 1, &m_vertexView);
    cmd->DrawInstanced(6, 1, 0, 0);
}