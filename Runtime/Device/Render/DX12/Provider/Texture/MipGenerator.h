#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include <dxcapi.h>

class Device;
class ShaderLibrary;
class CommandList;
class DescriptorAllocator;
class TextureResource;

using Microsoft::WRL::ComPtr;

class MipGenerator
{
public:
    ~MipGenerator();
    MipGenerator(Device& device);
    bool Initialize(ShaderLibrary& shaderLibrary);
    void GenerateMips(CommandList& cmd, DescriptorAllocator& srvAllocator, TextureResource* texResource);

private:
    bool LoadShader(ShaderLibrary& shaderLibrary);
    bool CreateRootSignature();
    bool CreatePSO();

private:
    Device& m_device;

    ComPtr<ID3D12RootSignature> m_rootSignature;
    ComPtr<ID3D12PipelineState> m_psoSRGB;
    ComPtr<ID3D12PipelineState> m_psoData;

    ComPtr<IDxcBlob> m_csSRGBBlob;
    ComPtr<IDxcBlob> m_csDataBlob;
};