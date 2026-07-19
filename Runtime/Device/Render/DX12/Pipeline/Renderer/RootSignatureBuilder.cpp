#include "pch.h"
#include "RootSignatureBuilder.h"
#include "Core/Device.h"
#include <dxcapi.h>

void RootSignatureBuilder::Add32BitConstants(UINT shaderRegister, UINT numConstants)
{
    m_constants.push_back({ shaderRegister, numConstants });
}

void RootSignatureBuilder::AddCBV(UINT shaderRegister)
{
    m_cbvs.push_back({ shaderRegister });
}

void RootSignatureBuilder::AddSRV(UINT shaderRegister, UINT registerSpace)
{
    m_srvs.push_back({ shaderRegister, registerSpace });
}

void RootSignatureBuilder::AddBindlessSRVTable(UINT numDescriptors, UINT baseRegister, UINT registerSpace)
{
    m_srvTables.push_back({ numDescriptors, baseRegister, registerSpace });
}

void RootSignatureBuilder::AddLinearSampler(UINT shaderRegister)
{
    m_samplers.push_back({ 
        shaderRegister,
        D3D12_FILTER_MIN_MAG_MIP_LINEAR 
        });
}

void RootSignatureBuilder::AddPointSampler(UINT shaderRegister)
{
    m_samplers.push_back({
        shaderRegister,
        D3D12_FILTER_MIN_MAG_MIP_POINT
        });
}

void RootSignatureBuilder::AddComparisonSampler(UINT shaderRegister)
{
    D3D12_STATIC_SAMPLER_DESC samplerDesc{};
    samplerDesc.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT; //비교(Comparison) 필터 설정
    samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER; // 그림자 맵 영역 밖 처리
    samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
    samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;

    // "현재 그리려는 깊이가 섀도우 맵의 깊이보다 작거나 같으면(앞에 있으면) 빛을 받는다"
    samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
    samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE; // 영역 밖은 흰색(그림자 없음) 처리
    samplerDesc.MinLOD = 0.0f;
    samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
    samplerDesc.ShaderRegister = shaderRegister; // 인자로 받은 레지스터 번호 (예: 1번이면 s1)
    samplerDesc.RegisterSpace = 0;
    samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // 픽셀 셰이더에서만 사용

    m_staticSamplers.push_back(samplerDesc);
}

void RootSignatureBuilder::AddFlags(D3D12_ROOT_SIGNATURE_FLAGS flags)
{
    m_flags |= flags;
}

ComPtr<ID3D12RootSignature> RootSignatureBuilder::Build(Device& device)
{
    std::vector<CD3DX12_DESCRIPTOR_RANGE> ranges;
    std::vector<CD3DX12_ROOT_PARAMETER> params;
    std::vector<CD3DX12_STATIC_SAMPLER_DESC> samplers;

    ranges.reserve(m_srvTables.size());
    params.reserve(m_constants.size() + m_cbvs.size() + m_srvs.size() + m_srvTables.size());
    samplers.reserve(m_samplers.size() + m_staticSamplers.size());

    // 일반적으로 가장 자주 바뀌는 루트 상수를 앞쪽 파라미터 색인에 두는 것이 성능상 유리.
    for (const auto& c : m_constants)
    {
        CD3DX12_ROOT_PARAMETER param;
        param.InitAsConstants(c.numConstants, c.shaderRegister);
        params.push_back(param);
    }

    for (const auto& c : m_cbvs)
    {
        CD3DX12_ROOT_PARAMETER param;
        param.InitAsConstantBufferView(c.shaderRegister);
        params.push_back(param);
    }

    for (const auto& s : m_srvs) // 힙을 거치지 않고 가상 주소를 다이렉트로 던질 버퍼들을 위한 통로. 주의! srvTable이랑 다름
    {
        CD3DX12_ROOT_PARAMETER param;
        param.InitAsShaderResourceView(s.shaderRegister, s.registerSpace);
        params.push_back(param);
    }

    for (const auto& t : m_srvTables) //6.6 이전에 쓰던 방식. 만약에 안쓰인다면 없애도 무방하다.
    {
        ranges.emplace_back();
        ranges.back().Init(
            D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 
            t.numDescriptors, 
            t.baseRegister, 
            t.registerSpace);
    }

    for (size_t i = 0; i < m_srvTables.size(); ++i)
    {
        CD3DX12_ROOT_PARAMETER param{};
        param.InitAsDescriptorTable(1, &ranges[i]);
        params.push_back(param);
    }

    for (const auto& s : m_samplers)
    {
        CD3DX12_STATIC_SAMPLER_DESC samplerDesc(s.shaderRegister, s.filter);
        samplers.push_back(samplerDesc);
    }
    samplers.insert(samplers.end(), m_staticSamplers.begin(), m_staticSamplers.end());

    D3D12_ROOT_SIGNATURE_FLAGS finalFlags = D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED;
    finalFlags |= m_flags;

    CD3DX12_ROOT_SIGNATURE_DESC desc;
    desc.Init(
        static_cast<UINT>(params.size()),
        params.data(),
        static_cast<UINT>(samplers.size()),
        samplers.empty() ? nullptr : samplers.data(),
        finalFlags);

    ComPtr<IDxcBlob> sig;
    ComPtr<IDxcBlob> err;

    HRESULT hr = D3D12SerializeRootSignature(
        &desc,
        D3D_ROOT_SIGNATURE_VERSION_1,
        reinterpret_cast<ID3DBlob**>(sig.GetAddressOf()),
        reinterpret_cast<ID3DBlob**>(err.GetAddressOf())
    );
    if (FAILED(hr))
    {
        if (err)
            OutputDebugStringA(static_cast<const char*>(err->GetBufferPointer()));
        return nullptr;
    }

    ComPtr<ID3D12RootSignature> rootSig;
    hr = device->CreateRootSignature(0, sig->GetBufferPointer(), sig->GetBufferSize(), IID_PPV_ARGS(&rootSig));
    Assert(SUCCEEDED(hr));

    return rootSig;
}