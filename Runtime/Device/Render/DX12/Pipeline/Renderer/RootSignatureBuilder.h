#pragma once
#include <d3d12.h>
#include <wrl.h>
#include "d3dx12.h"

class Device;

using Microsoft::WRL::ComPtr;

class RootSignatureBuilder
{
public:
    void Add32BitConstants(UINT shaderRegister, UINT numConstants);
    void AddCBV(UINT shaderRegister);
    void AddSRV(UINT shaderRegister, UINT registerSpace);
    void AddBindlessSRVTable(UINT numDescriptors, UINT baseRegister, UINT registerSpace); //6.6 이전에는 t레지스터를 이용해서 Bindless를 구현했는데, 그때 사용하는 함수. 구형버전이라 지워도 상관없음.
    
    void AddLinearSampler(UINT shaderRegister);
    void AddPointSampler(UINT shaderRegister);
    void AddComparisonSampler(UINT shaderRegister);
    void AddFlags(D3D12_ROOT_SIGNATURE_FLAGS flags);
    ComPtr<ID3D12RootSignature> Build(Device& device);

private:
    struct SRVTableDesc { UINT numDescriptors{ 0 }; UINT baseRegister{ 0 }; UINT registerSpace{ 0 }; };
    struct CBVDesc { UINT shaderRegister{ 0 }; };
    struct SRVDesc { UINT shaderRegister{ 0 }; UINT registerSpace{ 0 }; };
    struct ConstantDesc { UINT shaderRegister; UINT numConstants; };
    struct SamplerDesc { UINT shaderRegister{ 0 }; D3D12_FILTER filter; };

    std::vector<ConstantDesc> m_constants; //인덱스 같은 아주 가벼운 것들.
    std::vector<CBVDesc> m_cbvs; //전역적인 데이터들(빛이나 카메라같은)
    std::vector<SRVDesc> m_srvs; //물체들의 고유한 데이터들
    std::vector<SRVTableDesc> m_srvTables; //아주 큰 데이터(텍스쳐나 메쉬 같은). 6.6 이전에 쓰이던 것.
    std::vector<SamplerDesc> m_samplers;
    std::vector<D3D12_STATIC_SAMPLER_DESC> m_staticSamplers;
    D3D12_ROOT_SIGNATURE_FLAGS m_flags{ D3D12_ROOT_SIGNATURE_FLAG_NONE };
};