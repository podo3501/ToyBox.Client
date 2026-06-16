#pragma once
#include "RendererConfig.h"
#include "FrameUploadAllocator.h"
#include "Core/Math/Matrix.h"
#include "GameClient/Service/Render/Desc/RenderState.h"

struct DirectionalLightData;
class Device;
class PipelineCache;
class CommandList;
class ShaderProvider;
class MeshResource;

using Microsoft::WRL::ComPtr;

class ShadowRenderer
{
public:
    ~ShadowRenderer();
    ShadowRenderer() = delete;
    ShadowRenderer(const ShadowRendererConfig& config, PipelineCache& pipelineCache);

    bool Initialize(Device& device);
    void BindRootSignature(CommandList& cmd);
    void BindPipeline(CommandList& cmd);
    void PrepareFrame(const DirectionalLightData& light);
    void Draw(CommandList& cmd, MeshResource& mesh, const Core::Math::Matrix& world);

private:
    bool CreateRootSignature(Device& device);
    void CreateDefaultPSOs();
    void CreateConstantBuffers();
    ID3D12PipelineState* CreatePSO(const PipelineState& pipelineState);
    ID3D12PipelineState* GetPipeline(const PipelineState& pipelineState);

    D3D12_GPU_VIRTUAL_ADDRESS UpdateObjectCB(const Core::Math::Matrix& world);

    ShadowRendererConfig m_config;
    PipelineCache& m_pipelineCache;
    ComPtr<ID3D12RootSignature> m_rootSignature;
    std::optional<PipelineState> m_pipelineState;

    FrameUploadAllocator m_objectCBAllocator;
    FrameUploadAllocator m_frameCBAllocator;

    D3D12_GPU_VIRTUAL_ADDRESS m_frameCBAddress{ 0 };
};