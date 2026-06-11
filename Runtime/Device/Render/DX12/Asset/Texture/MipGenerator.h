#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include <dxcapi.h>

class ShaderSystem;
class CommandList;
class DescriptorAllocator;
class TextureResource;

using Microsoft::WRL::ComPtr;

class MipGenerator
{
public:
    ~MipGenerator();
    MipGenerator(ID3D12Device* device);
    bool Initialize(ShaderSystem* shaderSystem);
    void GenerateMips(CommandList& cmd, DescriptorAllocator* srvAllocator, TextureResource* texResource);

private:
    bool LoadShader(ShaderSystem* shaderSystem);
    bool CreateRootSignature();
    bool CreatePSO();

private:
    ID3D12Device* m_device{ nullptr };

    ComPtr<ID3D12RootSignature> m_rootSignature;
    ComPtr<ID3D12PipelineState> m_psoSRGB;
    ComPtr<ID3D12PipelineState> m_psoData;

    ComPtr<IDxcBlob> m_csSRGBBlob;
    ComPtr<IDxcBlob> m_csDataBlob;
};