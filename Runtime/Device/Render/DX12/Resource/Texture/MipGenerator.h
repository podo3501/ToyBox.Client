#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include <dxcapi.h>

class Device;
class ShaderProvider;
class CommandList;
class DescriptorAllocator;
class TextureResource;

using Microsoft::WRL::ComPtr;

class MipGenerator
{
public:
    ~MipGenerator();
    MipGenerator(Device& device);
    bool Initialize(ShaderProvider* shaderProvider);
    void GenerateMips(CommandList& cmd, DescriptorAllocator* srvAllocator, TextureResource* texResource);

private:
    bool LoadShader(ShaderProvider* shaderProvider);
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