#include "pch.h"
#include "MipGenerator.h"
#include "ShaderSystem.h"
#include "Descriptor/DescriptorAllocator.h"
#include "Command/CommandList.h"
#include "TextureResource.h"
#include "d3dx12.h"
#include <d3dcompiler.h>
#include <algorithm>

MipGenerator::~MipGenerator() = default;
MipGenerator::MipGenerator(ID3D12Device* device, DescriptorAllocator* srvAllocator) :
    m_device{ device },
    m_srvAllocator{ srvAllocator }
{}

bool MipGenerator::Initialize(ShaderSystem* shaderSystem)
{
    ReturnIfFalse(LoadShader(shaderSystem));
    ReturnIfFalse(CreateRootSignature());
    ReturnIfFalse(CreatePSO());

    return true;
}

bool MipGenerator::LoadShader(ShaderSystem* shaderSystem)
{
    ShaderVariant variant{ ShaderID::MipGenerator };
    const ShaderEntry* shaderEntry = shaderSystem->Find(variant);
    if (!shaderEntry)
        return false;

    m_csBlob = shaderEntry->cs;
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
    CD3DX12_ROOT_PARAMETER params[1] = {};

    // [전달할 데이터 구성 총 4 dwords]
    // constants[0] = SrcMipIndex (SRV 방 번호)
    // constants[1] = DstMipIndex (UAV 방 번호)
    // constants[2] = Width
    // constants[3] = Height
    params[0].InitAsConstants(4, 0); // b0 등록

    CD3DX12_ROOT_SIGNATURE_DESC desc{};
    desc.Init(1, params, 1, &sampler, D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED); //SM 6.6+ Bindless 힙 직접 인덱싱을 사용하기 위해 플래그를 명시
    
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

void MipGenerator::GenerateMips(CommandList& cmd, TextureResource* texResource)
{
    if (!texResource)
        return;

    ID3D12Resource* texture = texResource->Get();
    const D3D12_RESOURCE_DESC desc = texture->GetDesc();
    const UINT mipCount = desc.MipLevels;

    if (mipCount <= 1)
        return;

    ID3D12DescriptorHeap* heaps[] = { m_srvAllocator->GetHeap() };

    cmd->SetDescriptorHeaps(1, heaps);
    cmd->SetComputeRootSignature(m_rootSignature.Get());
    cmd->SetPipelineState(m_pso.Get());

    for (UINT srcMip = 0; srcMip < mipCount - 1; ++srcMip)
    {
        UINT dstMip = srcMip + 1;

        UINT srcMipSrvIndex = texResource->GetMipSrvIndex(srcMip);
        UINT dstMipUavIndex = texResource->GetMipUavIndex(dstMip);

        UINT width = std::max(1u, (UINT)(desc.Width >> dstMip));
        UINT height = std::max(1u, (UINT)(desc.Height >> dstMip));

        uint32_t constants[4] = {
            srcMipSrvIndex,  // 셰이더에서 읽을 소스 SRV 인덱스
            dstMipUavIndex,  // 셰이더에서 쓸 목적지 UAV 인덱스
            width,
            height
        };
        cmd->SetComputeRoot32BitConstants(0, 4, constants, 0);

        cmd->Dispatch((width + 7) / 8, (height + 7) / 8, 1);

        D3D12_RESOURCE_BARRIER barrier{};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
        barrier.UAV.pResource = texture;
        cmd->ResourceBarrier(1, &barrier);
    }
}


