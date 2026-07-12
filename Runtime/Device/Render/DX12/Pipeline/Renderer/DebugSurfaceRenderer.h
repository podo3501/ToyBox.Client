#pragma once
#include "RendererConfig.h"
#include "Allocator/FrameConstantAllocator.h"
#include "Core/Math/Matrix.h"
#include "GameClient/Service/Render/Desc/RenderState.h"

struct CameraData;
class Device;
class PipelineCache;
class CommandList;
class MeshResource;

using Microsoft::WRL::ComPtr;

class DebugSurfaceRenderer
{
public:
    ~DebugSurfaceRenderer();
    DebugSurfaceRenderer() = delete;
    DebugSurfaceRenderer(const DebugSurfaceRendererConfig& config, PipelineCache& pipelineCache);

    bool Initialize(Device& device);
    void PrepareFrame(const CameraData& camera);
    void BeginFrame(CommandList& cmd);
    void BindPipeline(CommandList& cmd, const PipelineState& pipelineState);
    void Draw(CommandList& cmd, MeshResource& mesh, const Core::Matrix& world);

private:
    enum class RootSlot : uint32_t
    {
        VertexIndex = 0,
        FrameCB = 1,
        ObjectCB = 2
    };

    bool CreateRootSignature(Device& device);
    bool CreateDefaultPSOs();
    ID3D12PipelineState* CreatePSO(const PipelineState& pipelineState);
    ID3D12PipelineState* GetPipeline(const PipelineState& pipelineState);

    D3D12_GPU_VIRTUAL_ADDRESS UploadObjectCB(const Core::Matrix& world);

    DebugSurfaceRendererConfig m_config;
    PipelineCache& m_pipelineCache;
    ComPtr<ID3D12RootSignature> m_rootSignature;
    ID3D12PipelineState* m_currentPSO{ nullptr };

    FrameConstantAllocator m_objectCBAllocator;
    FrameConstantAllocator m_frameCBAllocator;

    D3D12_GPU_VIRTUAL_ADDRESS m_frameCBAddress{};
};