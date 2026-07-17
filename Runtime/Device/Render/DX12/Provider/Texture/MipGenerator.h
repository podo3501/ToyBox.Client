#pragma once
#include "GameClient/Service/Render/Desc/TextureDesc.h"
#include <d3d12.h>
#include <wrl/client.h>
#include <dxcapi.h>

class Device;
class ShaderLibrary;
class CommandList;
class BindlessDescriptorAllocator;
class TextureResource;

using Microsoft::WRL::ComPtr;

enum class MipType : uint8_t
{
    SRGB = 0,
    Data,
    Count
};

class MipGenerator
{
public:
    ~MipGenerator();
    MipGenerator(Device& device);
    bool Initialize(ShaderLibrary& shaderLibrary);
    void GenerateMips(CommandList& cmd, BindlessDescriptorAllocator& srvAllocator, TextureResource* texResource);

private:
    enum class RootSlot : uint32_t
    {
        Constants = 0
    };

    bool LoadShader(ShaderLibrary& shaderLibrary);
    bool CreateRootSignature();
    bool CreatePSO();
    ID3D12PipelineState* GetPSO(MipType type) const;

    Device& m_device;
    ComPtr<ID3D12RootSignature> m_rootSignature;

    std::array<ComPtr<IDxcBlob>, Core::EnumSize<MipType>> m_shaderBlobs;
    std::array< ComPtr<ID3D12PipelineState>, Core::EnumSize<MipType>> m_psoMap;
};