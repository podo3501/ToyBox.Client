#pragma once
#include <d3d12.h>
#include <wrl.h>
#include "../d3dx12.h"

using Microsoft::WRL::ComPtr;

class RootSignatureBuilder
{
public:
    void AddSRVTable(UINT numDescriptors, UINT baseRegister);
    void AddCBV(UINT shaderRegister);
    void AddLinearSampler(UINT shaderRegister);
    ComPtr<ID3D12RootSignature> Build(ID3D12Device* device);

private:
    struct SRVTableDesc
    {
        UINT numDescriptors{ 0 };
        UINT baseRegister{ 0 };
    };

    struct CBVDesc
    {
        UINT shaderRegister{ 0 };
    };

    struct SamplerDesc
    {
        UINT shaderRegister{ 0 };
    };

    std::vector<SRVTableDesc> m_srvTables;
    std::vector<CBVDesc> m_cbvs;
    std::vector<SamplerDesc> m_samplers;
};