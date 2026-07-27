#include "pch.h"
#include "MipGenerator.h"
#include "Core/Device.h"
#include "Shader/ShaderLibrary.h"
#include "Allocator/BindlessDescriptorAllocator.h"
#include "Command/CommandList.h"
#include "Resource/Texture/TextureResource.h"
#include "Pipeline/Renderer/RootSignatureBuilder.h"

struct MipShaderDesc
{
    MipType type;
    std::vector<ShaderMacroDesc> macros;
};

static const MipShaderDesc g_mipShaders[] =
{
    { MipType::SRGB, {} },
    { MipType::Data, { {"IS_DATA_MAP"} } }
};

static MipType GetMipType(ColorSpace colorSpace)
{
    switch (colorSpace)
    {
    case ColorSpace::SRGB:        return MipType::SRGB;
    case ColorSpace::Linear:        return MipType::Data;
    default:
        return MipType::Data;
    }
}

MipGenerator::~MipGenerator() = default;
MipGenerator::MipGenerator(Device& device) :
    m_device{ device }
{}

bool MipGenerator::Initialize(ShaderLibrary& shaderLibrary)
{
    ReturnIfFalse(LoadShader(shaderLibrary));
    ReturnIfFalse(CreateRootSignature());
    ReturnIfFalse(CreatePSO());

    return true;
}

bool MipGenerator::LoadShader(ShaderLibrary& shaderLibrary)
{
    size_t loadedCount = 0;
    for (auto& desc : g_mipShaders)
    {
        ShaderVariant variant{ RegistryShader::MipGenerator };
        variant.runtimeMacros = desc.macros;

        if (const auto* entry = shaderLibrary.Find(variant))
        {
            m_shaderBlobs[Core::ToIndex(desc.type)] = entry->cs;
            ++loadedCount;
        }
    }

    return loadedCount == std::size(g_mipShaders);
}

bool MipGenerator::CreateRootSignature()
{
    RootSignatureBuilder builder;
    builder.Add32BitConstants(Core::ToIndex(RootSlot::Constants), 4);

    m_rootSignature = builder.Build(m_device);
    return m_rootSignature != nullptr;
}

bool MipGenerator::CreatePSO()
{
    D3D12_COMPUTE_PIPELINE_STATE_DESC desc{};
    desc.pRootSignature = m_rootSignature.Get();

    for (size_t i = 0; i < Core::EnumSize<MipType>; ++i)
    {
        auto& blob = m_shaderBlobs[i];
        if (!blob)
            continue;

        desc.CS = {
            blob->GetBufferPointer(),
            blob->GetBufferSize()
        };

        ReturnIfFailed(m_device->CreateComputePipelineState(&desc, IID_PPV_ARGS(&m_psoMap[i])));
    }

    return true;
}

ID3D12PipelineState* MipGenerator::GetPSO(MipType type) const
{
    return m_psoMap[Core::ToIndex(type)].Get();
}

void MipGenerator::GenerateMips(CommandList& cmd, BindlessDescriptorAllocator& srvAllocator, TextureResource* texResource)
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

    const MipType mipType = GetMipType(texResource->GetDesC().colorSpace);
    auto* pso = GetPSO(mipType);
    if (!pso) return;
    cmd->SetPipelineState(pso);

    for (UINT srcMip = 0; srcMip < mipCount - 1; ++srcMip)
    {
        UINT dstMip = srcMip + 1;

        UINT srcMipSrvIndex = texResource->GetMipSRVIndex(srcMip);
        UINT dstMipUavIndex = texResource->GetMipUAVIndex(dstMip);

        UINT width = std::max(1u, (UINT)(desc.Width >> dstMip));
        UINT height = std::max(1u, (UINT)(desc.Height >> dstMip));

        uint32_t constants[4] = {
            srcMipSrvIndex,  // 셰이더에서 읽을 소스 SRV 인덱스
            dstMipUavIndex,  // 셰이더에서 쓸 목적지 UAV 인덱스
            width,
            height
        };
        cmd->SetComputeRoot32BitConstants(Core::ToIndex(RootSlot::Constants), 4, constants, 0);

        cmd->Dispatch((width + 7) / 8, (height + 7) / 8, 1);

        D3D12_RESOURCE_BARRIER barrier{};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
        barrier.UAV.pResource = texRes.Get();
        cmd->ResourceBarrier(1, &barrier);
    }
}


