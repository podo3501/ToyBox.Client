#include "pch.h"
#include "MipGenerator.h"
#include "DescriptorAllocator.h"
#include "CommandList.h"
#include "d3dx12.h"
#include <d3dcompiler.h>
#include <algorithm>

MipGenerator::~MipGenerator() = default;
MipGenerator::MipGenerator(ID3D12Device* device, DescriptorAllocator* srvAllocator) :
    m_device{ device },
    m_srvAllocator{ srvAllocator }
{}

bool MipGenerator::Initialize()
{
    ReturnIfFalse(LoadShader());
    ReturnIfFalse(CreateRootSignature());
    ReturnIfFalse(CreatePSO());

    return true;
}

bool MipGenerator::LoadShader()
{
    ComPtr<ID3DBlob> error;
    wstring shaderFile = L"D:\\ProgrammingStudy\\ToyBox\\Runtime\\Device\\Render\\DX12\\MipGen.hlsl";
    HRESULT hr = D3DCompileFromFile(shaderFile.c_str(), nullptr, nullptr, "main", "cs_5_0",
        D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, &m_csBlob, &error);
    if (FAILED(hr))
    {
        if (error)
        {
            std::string msg(
                (char*)error->GetBufferPointer(),
                error->GetBufferSize()
            );

            OutputDebugStringA(msg.c_str());
        }
        return false;
    }

    return true;
}

CD3DX12_STATIC_SAMPLER_DESC sampler(
    0, // s0
    D3D12_FILTER_MIN_MAG_MIP_LINEAR,
    D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
    D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
    D3D12_TEXTURE_ADDRESS_MODE_CLAMP
);

bool MipGenerator::CreateRootSignature()
{
    CD3DX12_DESCRIPTOR_RANGE ranges[2] = {};
    ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
    ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);

    CD3DX12_ROOT_PARAMETER params[3] = {};
    params[0].InitAsDescriptorTable(1, &ranges[0]);
    params[1].InitAsDescriptorTable(1, &ranges[1]);
    params[2].InitAsConstants(2, 0); // Root Constants (b0)

    CD3DX12_ROOT_SIGNATURE_DESC desc{};
    desc.Init(3, params, 1, &sampler, D3D12_ROOT_SIGNATURE_FLAG_NONE);

    ComPtr<ID3DBlob> sig;
    ComPtr<ID3DBlob> err;

    ReturnIfFailed(D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &sig, &err));
    ReturnIfFailed(m_device->CreateRootSignature(0, 
        sig->GetBufferPointer(), 
        sig->GetBufferSize(), 
        IID_PPV_ARGS(&m_rootSignature)));

    return true;
}

bool MipGenerator::CreatePSO()
{
    D3D12_COMPUTE_PIPELINE_STATE_DESC desc{};
    desc.pRootSignature = m_rootSignature.Get();

    desc.CS = { m_csBlob->GetBufferPointer(), m_csBlob->GetBufferSize() };
    ReturnIfFailed(m_device->CreateComputePipelineState(&desc, IID_PPV_ARGS(&m_pso)));

    return true;
}

void MipGenerator::GenerateMips(CommandList& cmd, ID3D12Resource* texture)
{
    const D3D12_RESOURCE_DESC desc = texture->GetDesc();
    const UINT mipCount = desc.MipLevels;

    if (mipCount <= 1)
        return;

    ID3D12DescriptorHeap* heaps[] = {
        m_srvAllocator->GetHeap()
    };

    cmd->SetDescriptorHeaps(1, heaps);
    cmd->SetComputeRootSignature(m_rootSignature.Get());
    cmd->SetPipelineState(m_pso.Get());

    for (UINT srcMip = 0; srcMip < mipCount - 1; ++srcMip)
    {
        UINT dstMip = srcMip + 1;

        auto srv = m_srvAllocator->Allocate();
        auto uav = m_srvAllocator->Allocate();

        auto srvCpu = srv.GetCpuHandle();
        auto srvGpu = srv.GetGpuHandle();

        auto uavCpu = uav.GetCpuHandle();
        auto uavGpu = uav.GetGpuHandle();

        // SRV (source mip)
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Format = desc.Format;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip = srcMip;
        srvDesc.Texture2D.MipLevels = 1;

        m_device->CreateShaderResourceView(texture, &srvDesc, srvCpu);

        // UAV (dest mip)
        D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
        uavDesc.Format = desc.Format;
        uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
        uavDesc.Texture2D.MipSlice = dstMip;

        m_device->CreateUnorderedAccessView(texture, nullptr, &uavDesc, uavCpu);

        cmd->SetComputeRootDescriptorTable(0, srvGpu);
        cmd->SetComputeRootDescriptorTable(1, uavGpu);

        UINT width = std::max(1u, (UINT)(desc.Width >> dstMip));
        UINT height = std::max(1u, (UINT)(desc.Height >> dstMip));
        uint32_t constants[2] = { width, height };
        cmd->SetComputeRoot32BitConstants(2, 2, constants, 0);

        cmd->Dispatch((width + 7) / 8, (height + 7) / 8, 1);

        D3D12_RESOURCE_BARRIER barrier{};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
        barrier.UAV.pResource = texture;

        cmd->ResourceBarrier(1, &barrier);

        cmd.EnqueueDeferredDescriptors(move(srv));
        cmd.EnqueueDeferredDescriptors(move(uav));
    }
}


