#include "pch.h"
#include "MeshRenderer.h"
#include "MeshResource.h"
#include "d3dx12.h"
#include <d3dcompiler.h>
#include "CommandList.h"
#include "DescriptorAllocation.h"
#include "DX12MathUtils.h"

namespace cm = Core::Math;

struct ObjectCB
{
    float world[16];
};

struct FrameCB
{
    float view[16];
    float proj[16];
};

MeshRenderer::~MeshRenderer() = default;
MeshRenderer::MeshRenderer(ID3D12Device* device) :
    m_device{ device }
{}

bool MeshRenderer::Initialize(const Size& size)
{
    m_screenSize = size;

    CreateRootSignature();
    CreateDefaultPSOs();
    CreateConstantBuffers();

    return true;
}

void MeshRenderer::SetRasterState(const RasterState& rasterState)
{
    m_rasterState = rasterState;
}

void MeshRenderer::CreateDefaultPSOs()
{
    CreatePipeline({ FillMode::Solid, CullMode::Back });
    CreatePipeline({ FillMode::Solid, CullMode::None });
    CreatePipeline({ FillMode::Wireframe, CullMode::None });
}

ID3D12PipelineState* MeshRenderer::GetPipeline(const PSOKey& key)
{
    auto it = m_psoCache.find(key);
    if (it != m_psoCache.end())
        return it->second.Get();

    CreatePipeline(key);

    return m_psoCache[key].Get();
}

void MeshRenderer::CreatePipeline(const PSOKey& key)
{
    ComPtr<ID3DBlob> vs;
    ComPtr<ID3DBlob> ps;
    ComPtr<ID3DBlob> err;

    std::wstring shaderFile = L"D:\\ProgrammingStudy\\ToyBox\\Runtime\\Device\\Render\\DX12\\Mesh.hlsl";
    D3DCompileFromFile(shaderFile.c_str(), nullptr, nullptr, "VSMain", "vs_5_0", 0, 0, &vs, &err);
    D3DCompileFromFile(shaderFile.c_str(), nullptr, nullptr, "PSMain", "ps_5_0", 0, 0, &ps, &err);

    D3D12_GRAPHICS_PIPELINE_STATE_DESC pso{};
    pso.InputLayout = { nullptr, 0 };
    pso.pRootSignature = m_rootSignature.Get();

    pso.VS = { vs->GetBufferPointer(), vs->GetBufferSize() };
    pso.PS = { ps->GetBufferPointer(), ps->GetBufferSize() };

    //pso.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    CD3DX12_RASTERIZER_DESC raster(D3D12_DEFAULT);

    raster.FillMode =
        key.fillMode == FillMode::Wireframe
        ? D3D12_FILL_MODE_WIREFRAME
        : D3D12_FILL_MODE_SOLID;

    switch (key.cullMode)
    {
    case CullMode::None: raster.CullMode = D3D12_CULL_MODE_NONE; break;
    case CullMode::Front: raster.CullMode = D3D12_CULL_MODE_FRONT; break;
    case CullMode::Back: raster.CullMode = D3D12_CULL_MODE_BACK; break;
    }

    pso.RasterizerState = raster;
    pso.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

    //pso.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    pso.DepthStencilState.DepthEnable = FALSE;

    pso.SampleMask = UINT_MAX;
    pso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

    pso.NumRenderTargets = 1;
    pso.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

    pso.DSVFormat = DXGI_FORMAT_UNKNOWN;
    //pso.DSVFormat = DXGI_FORMAT_D32_FLOAT;

    pso.SampleDesc.Count = 1;

    ComPtr<ID3D12PipelineState> pipeline;
    m_device->CreateGraphicsPipelineState(&pso, IID_PPV_ARGS(&pipeline));

    m_psoCache[key] = pipeline;
}

void MeshRenderer::CreateRootSignature()
{
    CD3DX12_DESCRIPTOR_RANGE rangeMesh;
    rangeMesh.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 0); // vb, ib

    CD3DX12_DESCRIPTOR_RANGE rangeTex;
    rangeTex.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2); // vb, ib

    CD3DX12_ROOT_PARAMETER params[4];
    params[0].InitAsDescriptorTable(1, &rangeMesh); // Mesh Table (t0-t1)
    params[1].InitAsDescriptorTable(1, &rangeTex);  // Texture Table (t2)
    params[2].InitAsConstantBufferView(0);      // object
    params[3].InitAsConstantBufferView(1);      // frame

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
    auto desc = CD3DX12_RESOURCE_DESC::Buffer(256);

    // Object CB
    for (uint32_t i = 0; i < kMaxObjectCount; ++i)
    {
        m_device->CreateCommittedResource(
            &heap,
            D3D12_HEAP_FLAG_NONE,
            &desc,
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
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_frameCB)
    );

    m_frameCB->Map(0, nullptr, (void**)&m_frameData);
}

void MeshRenderer::BindPipeline(CommandList& cmd)
{
    cmd->SetGraphicsRootSignature(
        m_rootSignature.Get());

    PSOKey key;
    key.fillMode = m_rasterState.fillMode;
    key.cullMode = m_rasterState.cullMode;

    auto* pso = GetPipeline(key);
    cmd->SetPipelineState(pso);
}

void MeshRenderer::BindDescriptorHeap(CommandList& cmd)
{
    ID3D12DescriptorHeap* heaps[] = { m_srvHeap };
    cmd->SetDescriptorHeaps(1, heaps);
}

void MeshRenderer::SetFrameCB(const FrameCB& frame)
{
    *m_frameData = frame;
}

void MeshRenderer::BeginFrame()
{
    m_objectIndex = 0;
}

void MeshRenderer::Draw(CommandList& cmd, MeshResource& mesh, const cm::Matrix& world, DescriptorAllocation& textureSrv)
{
    UpdateFrameCB();
    auto objectCBAddress = UpdateObjectCB(world);
    auto& meshTable = mesh.GetMeshTable();
    
    cmd->SetGraphicsRootDescriptorTable(0, meshTable.GetGpuHandle());
    //cmd->SetGraphicsRootDescriptorTable(1, textureSrv.GetGpuHandle());

    cmd->SetGraphicsRootConstantBufferView(2, objectCBAddress);
    cmd->SetGraphicsRootConstantBufferView(3, m_frameCB->GetGPUVirtualAddress());

    cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    cmd->DrawInstanced(mesh.GetIndexCount(), 1, 0, 0);
    //cmd->DrawIndexedInstanced(mesh.GetIndexCount(), 1, 0, 0, 0);

    meshTable.MarkUsed(cmd.GetType(), cmd.GetFence());
    //textureSrv.MarkUsed(cmd.GetType(), cmd.GetFence());
}

void MeshRenderer::UpdateFrameCB()
{
    FrameCB frame{};

    XMMATRIX view =
        XMMatrixLookAtLH(
            XMVectorSet(0.0f, 0.0f, -5.0f, 1.0f),
            XMVectorZero(),
            XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
        );

    float aspect = (m_screenSize.height == 0) ? 1.0f : (float)m_screenSize.width / (float)m_screenSize.height;

    XMMATRIX proj =
        XMMatrixPerspectiveFovLH(
            XM_PIDIV4,
            aspect,
            0.1f,
            1000.0f
        );

    XMStoreFloat4x4((XMFLOAT4X4*)frame.view, XMMatrixTranspose(view));
    XMStoreFloat4x4((XMFLOAT4X4*)frame.proj, XMMatrixTranspose(proj));

    *m_frameData = frame;
}

D3D12_GPU_VIRTUAL_ADDRESS MeshRenderer::UpdateObjectCB(const cm::Matrix& world)
{
    Assert(m_objectIndex < kMaxObjectCount);

    ObjectCB obj{};

    DirectX::XMMATRIX xmWorld = ToDXMatrix(world);
    DirectX::XMStoreFloat4x4(
        reinterpret_cast<DirectX::XMFLOAT4X4*>(obj.world),
        DirectX::XMMatrixTranspose(xmWorld));

    *m_objectDatas[m_objectIndex] = obj;
    auto gpuAddress = m_objectCBs[m_objectIndex]->GetGPUVirtualAddress();

    ++m_objectIndex;
    return gpuAddress;
}

void MeshRenderer::Resize(const Size& size)
{
    m_screenSize = size;
}