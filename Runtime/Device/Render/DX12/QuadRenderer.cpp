#include "pch.h"
#include "QuadRenderer.h"
#include "d3dx12.h"
#include <d3dcompiler.h>
#include "Vertex.h"

struct QuadTransform
{
    float scale[2];
    float offset[2];
};

using Microsoft::WRL::ComPtr;

QuadRenderer::~QuadRenderer() = default;
QuadRenderer::QuadRenderer(const DX12DeviceView& dv) : 
    m_dv{ dv }
{}

bool QuadRenderer::Initialize(const Size& screenSize)
{
    HRESULT hr;
    m_screenSize = screenSize;

    CD3DX12_DESCRIPTOR_RANGE range;
    range.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0

    CD3DX12_ROOT_PARAMETER params[2] = {};
    params[0].InitAsDescriptorTable(1, &range);
    params[1].InitAsConstantBufferView(0);

    CD3DX12_STATIC_SAMPLER_DESC sampler(
        0, // s0
        D3D12_FILTER_MIN_MAG_MIP_LINEAR
    );

    CD3DX12_ROOT_SIGNATURE_DESC rsDesc;
    rsDesc.Init(2, params, 1, &sampler,
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    ComPtr<ID3DBlob> sig{ nullptr };
    ComPtr<ID3DBlob> err{ nullptr };

    D3D12SerializeRootSignature(&rsDesc, D3D_ROOT_SIGNATURE_VERSION_1, &sig, &err);

    m_dv.device->CreateRootSignature(
        0,
        sig->GetBufferPointer(),
        sig->GetBufferSize(),
        IID_PPV_ARGS(&m_rootSignature)
    );

    ComPtr<ID3DBlob> vs{ nullptr };
    ComPtr<ID3DBlob> ps{ nullptr };

    wstring shaderFile = L"D:\\ProgrammingStudy\\ToyBox\\Runtime\\Device\\Render\\DX12\\Quad.hlsl";
    hr = D3DCompileFromFile(shaderFile.c_str(), nullptr, nullptr, "VSMain", "vs_5_0", 0, 0, &vs, &err);
    if (FAILED(hr)) return false;

    hr = D3DCompileFromFile(shaderFile.c_str(), nullptr, nullptr, "PSMain", "ps_5_0", 0, 0, &ps, &err);
    if (FAILED(hr)) return false;

    D3D12_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },

        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },

        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28,
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

    hr = m_dv.device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState));
    if (FAILED(hr)) return false;

    // Constant Buffer 생성
    CD3DX12_HEAP_PROPERTIES heap(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(256); // 256 align 필수

    HRESULT hrCB = m_dv.device->CreateCommittedResource(
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

void QuadRenderer::UploadQuad(ID3D12GraphicsCommandList* uploadCmd)
{
    auto device = m_dv.device;

    Vertex quad[6] =
    {
        { -0.5f, -0.5f, 0, 1,1,1,1, 0,1 },
        { -0.5f,  0.5f, 0, 1,1,1,1, 0,0 },
        {  0.5f, -0.5f, 0, 1,1,1,1, 1,1 },

        {  0.5f, -0.5f, 0, 1,1,1,1, 1,1 },
        { -0.5f,  0.5f, 0, 1,1,1,1, 0,0 },
        {  0.5f,  0.5f, 0, 1,1,1,1, 1,0 },
    };

    UINT bufferSize = sizeof(quad);

    // 1. GPU 전용 DEFAULT 버퍼
    CD3DX12_HEAP_PROPERTIES defaultHeap(D3D12_HEAP_TYPE_DEFAULT);
    CD3DX12_RESOURCE_DESC vbDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

    device->CreateCommittedResource(
        &defaultHeap,
        D3D12_HEAP_FLAG_NONE,
        &vbDesc,
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS(&m_vertexBuffer)
    );

    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_vertexBuffer.Get(),
        D3D12_RESOURCE_STATE_COMMON,
        D3D12_RESOURCE_STATE_COPY_DEST
    );
    uploadCmd->ResourceBarrier(1, &barrier);

    // 2. CPU 접근용 UPLOAD 버퍼
    CD3DX12_HEAP_PROPERTIES uploadHeap(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC uploadDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

    device->CreateCommittedResource(
        &uploadHeap,
        D3D12_HEAP_FLAG_NONE,
        &uploadDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_uploadBuffer)
    );

    // 3. 데이터 복사
    void* data;
    m_uploadBuffer->Map(0, nullptr, &data);
    memcpy(data, quad, bufferSize);
    m_uploadBuffer->Unmap(0, nullptr);

    // 4. GPU 복사
    uploadCmd->CopyBufferRegion(m_vertexBuffer.Get(), 0, m_uploadBuffer.Get(), 0, bufferSize);

    m_vertexView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
    m_vertexView.SizeInBytes = bufferSize;
    m_vertexView.StrideInBytes = sizeof(Vertex);
}

void QuadRenderer::BindPipeline(ID3D12GraphicsCommandList* cmd)
{
    cmd->SetGraphicsRootSignature(m_rootSignature.Get());
    cmd->SetPipelineState(m_pipelineState.Get());
}

void QuadRenderer::TransitionToRenderState(ID3D12GraphicsCommandList* cmd)
{
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_vertexBuffer.Get(),
        D3D12_RESOURCE_STATE_COPY_DEST,
        D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
    );
    cmd->ResourceBarrier(1, &barrier);
}

void QuadRenderer::Draw( ID3D12GraphicsCommandList* cmd, const Rect& dest,
    const CD3DX12_GPU_DESCRIPTOR_HANDLE& gpuHandle)
{
    ID3D12DescriptorHeap* heaps[] = { m_srvHeap };
    cmd->SetDescriptorHeaps(1, heaps);
    cmd->SetGraphicsRootDescriptorTable(0, gpuHandle);

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

    cmd->SetGraphicsRootConstantBufferView(1, m_constantBuffer->GetGPUVirtualAddress());
    cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmd->IASetVertexBuffers(0, 1, &m_vertexView);
    cmd->DrawInstanced(6, 1, 0, 0);
}

void QuadRenderer::SetSRVHeap(ID3D12DescriptorHeap* heap)
{
    m_srvHeap = heap;
}