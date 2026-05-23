#include "pch.h"
#include "RootSignatureBuilder.h"

void RootSignatureBuilder::AddSRVTable(UINT numDescriptors, UINT baseRegister)
{
    m_srvTables.push_back({ numDescriptors, baseRegister });
}

void RootSignatureBuilder::AddCBV(UINT shaderRegister)
{
    m_cbvs.push_back({ shaderRegister });
}

void RootSignatureBuilder::AddLinearSampler(UINT shaderRegister)
{
    m_samplers.push_back({ shaderRegister });
}

ComPtr<ID3D12RootSignature> RootSignatureBuilder::Build(ID3D12Device* device)
{
    std::vector<CD3DX12_DESCRIPTOR_RANGE> ranges;
    std::vector<CD3DX12_ROOT_PARAMETER> params;
    std::vector<CD3DX12_STATIC_SAMPLER_DESC> samplers;

    ranges.reserve(m_srvTables.size());
    params.reserve(m_srvTables.size() + m_cbvs.size());
    samplers.reserve(m_samplers.size());

    // 1. SRV Tables
    for (const auto& t : m_srvTables)
    {
        ranges.emplace_back();
        ranges.back().Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, t.numDescriptors, t.baseRegister);

        CD3DX12_ROOT_PARAMETER param;
        param.InitAsDescriptorTable(1, &ranges.back());
        params.push_back(param);
    }

    // 2. CBVs
    for (const auto& c : m_cbvs)
    {
        CD3DX12_ROOT_PARAMETER param;
        param.InitAsConstantBufferView(c.shaderRegister);
        params.push_back(param);
    }

    // 3. Samplers
    for (const auto& s : m_samplers)
        samplers.emplace_back(s.shaderRegister, D3D12_FILTER_MIN_MAG_MIP_LINEAR);

    CD3DX12_ROOT_SIGNATURE_DESC desc;
    desc.Init(
        static_cast<UINT>(params.size()),
        params.data(),
        static_cast<UINT>(samplers.size()),
        samplers.empty() ? nullptr : samplers.data(),
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    ComPtr<ID3DBlob> sig;
    ComPtr<ID3DBlob> err;

    HRESULT hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &sig, &err);
    if (FAILED(hr))
    {
        if (err)
            OutputDebugStringA(static_cast<const char*>(err->GetBufferPointer()));
        return nullptr;
    }

    ComPtr<ID3D12RootSignature> rootSig;
    device->CreateRootSignature(0, sig->GetBufferPointer(), sig->GetBufferSize(), IID_PPV_ARGS(&rootSig));

    return rootSig;
}