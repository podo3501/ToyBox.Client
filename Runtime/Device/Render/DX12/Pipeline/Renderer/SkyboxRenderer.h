#pragma once
#include "RendererConfig.h"
#include "Allocator/FrameConstantAllocator.h"
#include <d3d12.h>
#include <wrl.h>

class Device;
class PipelineCache;
class CommandList;
class TextureCubeResource;
struct CameraData;

using Microsoft::WRL::ComPtr;

class SkyboxRenderer
{
public:
    ~SkyboxRenderer();
    SkyboxRenderer(const SkyboxRendererConfig& config, PipelineCache& pipelineCache);

    bool Initialize(Device& device);
    void ResetFrameResources();

    void Draw(CommandList& cmd, const CameraData& camera, TextureCubeResource& skybox);

private:
    enum class RootSlot : uint32_t
    {
        FrameCB = 0,
    };

    bool CreateRootSignature(Device& device);
    ID3D12PipelineState* CreatePSO();

    SkyboxRendererConfig m_config;
    PipelineCache& m_pipelineCache;

    ComPtr<ID3D12RootSignature> m_rootSignature;
    ID3D12PipelineState* m_pso{ nullptr };

    FrameConstantAllocator m_cbAllocator;
};