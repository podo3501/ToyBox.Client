#include "pch.h"
#include "MipGenerator.h"
#include "Core/Device.h"
#include "Resource/Shader/ShaderProvider.h"
#include "Factory/DescriptorAllocator.h"
#include "Command/CommandList.h"
#include "TextureResource.h"
#include "Renderer/RootSignatureBuilder.h"

MipGenerator::~MipGenerator() = default;
MipGenerator::MipGenerator(Device& device) :
    m_device{ device }
{}

bool MipGenerator::Initialize(ShaderProvider& shaderProvider)
{
    ReturnIfFalse(LoadShader(shaderProvider));
    ReturnIfFalse(CreateRootSignature());
    ReturnIfFalse(CreatePSO());

    return true;
}

bool MipGenerator::LoadShader(ShaderProvider& shaderProvider)
{
    ShaderVariant sRGBVariant{ ShadingModel::MipGenerator };
    if (const auto* entry = shaderProvider.Find(sRGBVariant))
        m_csSRGBBlob = entry->cs;

    ShaderVariant srgbVariant{ ShadingModel::MipGenerator };
    srgbVariant.runtimeMacros.push_back({ "IS_DATA_MAP" });
    if (const auto* entry = shaderProvider.Find(srgbVariant))
        m_csDataBlob = entry->cs;

    return (m_csSRGBBlob && m_csDataBlob);
}

bool MipGenerator::CreateRootSignature()
{
    RootSignatureBuilder builder;
    builder.Add32BitConstants(0, 4);

    m_rootSignature = builder.Build(m_device);
    return m_rootSignature != nullptr;
}

bool MipGenerator::CreatePSO()
{
    D3D12_COMPUTE_PIPELINE_STATE_DESC desc{};
    desc.pRootSignature = m_rootSignature.Get();

    desc.CS = { m_csSRGBBlob->GetBufferPointer(), m_csSRGBBlob->GetBufferSize() };
    ReturnIfFailed(m_device->CreateComputePipelineState(&desc, IID_PPV_ARGS(&m_psoSRGB)));

    desc.CS = { m_csDataBlob->GetBufferPointer(), m_csDataBlob->GetBufferSize() };
    ReturnIfFailed(m_device->CreateComputePipelineState(&desc, IID_PPV_ARGS(&m_psoData)));

    return true;
}

void MipGenerator::GenerateMips(CommandList& cmd, DescriptorAllocator& srvAllocator, TextureResource* texResource)
{
    if (!texResource)
        return;

    auto& texRes = texResource->Get();
    const D3D12_RESOURCE_DESC desc = texRes->GetDesc();
    const UINT mipCount = desc.MipLevels;

    if (mipCount <= 1)
        return;

    ID3D12DescriptorHeap* heaps[] = { srvAllocator.GetHeap() };

    cmd->SetDescriptorHeaps(1, heaps);
    cmd->SetComputeRootSignature(m_rootSignature.Get());
    if(texResource->GetDesc().srgb)
        cmd->SetPipelineState(m_psoSRGB.Get());
    else
        cmd->SetPipelineState(m_psoData.Get());

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
        barrier.UAV.pResource = texRes.Get();
        cmd->ResourceBarrier(1, &barrier);
    }
}


