#pragma once
#include <d3d12.h>
#include <wrl/client.h>

class ShaderSystem;
class CommandList;
class DescriptorAllocator;
class DescriptorAllocation;

using Microsoft::WRL::ComPtr;

class MipGenerator
{
public:
    ~MipGenerator();
    MipGenerator(ID3D12Device* device, DescriptorAllocator* srvAllocator);
    bool Initialize(ShaderSystem* shaderSystem);
    void GenerateMips(CommandList& cmd, ID3D12Resource* texture);

private:
    bool LoadShader(ShaderSystem* shaderSystem);
    bool CreateRootSignature();
    bool CreatePSO();

private:
    ID3D12Device* m_device{ nullptr };
    DescriptorAllocator* m_srvAllocator{ nullptr };

    ComPtr<ID3D12RootSignature> m_rootSignature;
    ComPtr<ID3D12PipelineState> m_pso;
    ComPtr<ID3DBlob> m_csBlob;
};