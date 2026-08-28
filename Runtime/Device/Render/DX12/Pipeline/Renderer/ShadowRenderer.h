#pragma once
#include "RendererConfig.h"
#include "Allocator/FrameConstantAllocator.h"
#include "Core/Math/Matrix.h"
#include "Definition/RenderState.h"

struct DirectionalLightData;
class Device;
class PipelineCache;
class CommandList;
class MeshResource;

using Microsoft::WRL::ComPtr;

class ShadowRenderer
{
public:
    ~ShadowRenderer();
    ShadowRenderer() = delete;
    ShadowRenderer(const ShadowRendererConfig& config, PipelineCache& pipelineCache);

    bool Initialize(Device& device);
    void ResetFrameResources();

    void PrepareDraw(CommandList& cmd, const DirectionalLightData& light);
    void Draw(CommandList& cmd, MeshResource& mesh, const Core::Matrix& world);

private:
    enum class RootSlot : uint32_t
    {
        MeshData = 0,
        FrameCB = 1,
        ObjectCB = 2
    };

    bool CreateRootSignature(Device& device);
    ID3D12PipelineState* CreatePSO(const PipelineState& pipelineState);

    D3D12_GPU_VIRTUAL_ADDRESS UploadFrameCB(const DirectionalLightData& light);
    D3D12_GPU_VIRTUAL_ADDRESS UploadObjectCB(const Core::Matrix& world);

    ShadowRendererConfig m_config;
    PipelineCache& m_pipelineCache;
    ComPtr<ID3D12RootSignature> m_rootSignature;
    ID3D12PipelineState* m_shadowPSO{ nullptr };

    FrameConstantAllocator m_objectCBAllocator;
    FrameConstantAllocator m_frameCBAllocator;
};