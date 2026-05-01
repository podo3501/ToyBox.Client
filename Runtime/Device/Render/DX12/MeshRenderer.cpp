#include "pch.h"
#include "MeshRenderer.h"
#include "MeshResource.h"
#include "d3dx12.h"
#include <d3dcompiler.h>
#include "CommandList.h"
#include "DescriptorAllocation.h"

struct ObjectCB
{
    float world[16];
};

struct FrameCB
{
    float view[16];
    float proj[16];
};

bool MeshRenderer::Initialize(ID3D12Device* device)
{
    CreateRootSignature(device);
    CreatePipeline(device);
    CreateConstantBuffers(device);
    return true;
}

void MeshRenderer::CreateRootSignature(ID3D12Device* device)
{
    CD3DX12_DESCRIPTOR_RANGE range;
    range.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0

    CD3DX12_ROOT_PARAMETER params[3];

    params[0].InitAsDescriptorTable(1, &range); // texture
    params[1].InitAsConstantBufferView(0);      // object
    params[2].InitAsConstantBufferView(1);      // frame

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

    device->CreateRootSignature(
        0,
        sig->GetBufferPointer(),
        sig->GetBufferSize(),
        IID_PPV_ARGS(&m_rootSignature)
    );
}

void MeshRenderer::CreatePipeline(ID3D12Device* device)
{
    ComPtr<ID3DBlob> vs;
    ComPtr<ID3DBlob> ps;
    ComPtr<ID3DBlob> err;

    std::wstring shaderFile = L"D:\\ProgrammingStudy\\ToyBox\\Runtime\\Device\\Render\\DX12\\Mesh.hlsl";
    D3DCompileFromFile(shaderFile.c_str(), nullptr, nullptr, "VSMain", "vs_5_0", 0, 0, &vs, &err);
    D3DCompileFromFile(shaderFile.c_str(), nullptr, nullptr, "PSMain", "ps_5_0", 0, 0, &ps, &err);

    D3D12_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },

        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },

        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };

    D3D12_GRAPHICS_PIPELINE_STATE_DESC pso{};
    pso.InputLayout = { layout, _countof(layout) };
    pso.pRootSignature = m_rootSignature.Get();

    pso.VS = { vs->GetBufferPointer(), vs->GetBufferSize() };
    pso.PS = { ps->GetBufferPointer(), ps->GetBufferSize() };

    pso.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    pso.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

    pso.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

    pso.SampleMask = UINT_MAX;
    pso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

    pso.NumRenderTargets = 1;
    pso.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

    pso.DSVFormat = DXGI_FORMAT_D32_FLOAT;

    pso.SampleDesc.Count = 1;

    device->CreateGraphicsPipelineState(
        &pso,
        IID_PPV_ARGS(&m_pipelineState)
    );
}

void MeshRenderer::CreateConstantBuffers(ID3D12Device* device)
{
    CD3DX12_HEAP_PROPERTIES heap(D3D12_HEAP_TYPE_UPLOAD);
    auto desc = CD3DX12_RESOURCE_DESC::Buffer(256);

    // Object CB
    device->CreateCommittedResource(
        &heap,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_objectCB)
    );

    m_objectCB->Map(0, nullptr, (void**)&m_objectData);

    // Frame CB
    device->CreateCommittedResource(
        &heap,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_frameCB)
    );

    m_frameCB->Map(0, nullptr, (void**)&m_frameData);
}

void MeshRenderer::BindPipeline(CommandList& cmd)
{
    cmd->SetGraphicsRootSignature(m_rootSignature.Get());
    cmd->SetPipelineState(m_pipelineState.Get());
}

void MeshRenderer::SetFrameCB(const FrameCB& frame)
{
    *m_frameData = frame;
}

void MeshRenderer::Draw(CommandList& cmd, MeshResource& mesh, DescriptorAllocation& srv, const ObjectCB& obj)
{
    *m_objectData = obj;

    auto handle = srv.GetGpuHandle();
    cmd->SetGraphicsRootDescriptorTable(0, handle);
    srv.MarkUsed(cmd.GetType(), cmd.GetFence());

    cmd->SetGraphicsRootConstantBufferView(1, m_objectCB->GetGPUVirtualAddress());
    cmd->SetGraphicsRootConstantBufferView(2, m_frameCB->GetGPUVirtualAddress());

    cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmd->IASetVertexBuffers(0, 1, &mesh.GetVBView());
    cmd->IASetIndexBuffer(&mesh.GetIBView());

    cmd->DrawIndexedInstanced(mesh.GetIndexCount(), 1, 0, 0, 0);
}