#pragma once
#include "RendererConfig.h"
#include "Allocator/FrameConstantAllocator.h"
#include "Core/Foundation/Geometry2D.h"
#include "Core/Math/Vector4.h"
#include "Core/Math/Matrix.h"
#include "GameClient/Service/Render/Desc/RenderState.h"

class Device;
class PipelineCache;
class CommandList;
class MeshResource;
class UIMaterialResource;
class FrameConstantAllocator;

class UIRenderer
{
public:
    ~UIRenderer();
    UIRenderer(const UIRendererConfig& config, PipelineCache& pipelineCache);
    bool Initialize(Device& device, const Size& screenSize);
    void PrepareFrame();
    void BeginFrame(CommandList& cmd);
    void BindPipeline(CommandList& cmd, const PipelineState& pipelineState);

    void Draw(
        CommandList& cmd, 
        MeshResource& mesh, 
        UIMaterialResource& material, 
        const Core::Matrix& world,
        const Core::Vector4& uvTransform);

    void SetScreenSize(const Size& size);

private:
    enum class RootSlot : uint32_t
    {
        ResourceIndices = 0, //vb index, ib index, tex index
        DrawCB = 1
    };

    bool CreateRootSignature(Device& device);
    bool CreateDefaultPSOs();
    ID3D12PipelineState* CreatePSO(const PipelineState& pipelineState);
    ID3D12PipelineState* GetPipeline(const PipelineState& pipelineState);
    D3D12_GPU_VIRTUAL_ADDRESS UploadDrawCB(
        UIMaterialResource& material,
        const Core::Matrix& world, 
        const Core::Vector4& uvTransform);

    UIRendererConfig m_config;
    PipelineCache& m_pipelineCache;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;
    ID3D12PipelineState* m_currentPSO{ nullptr };

    FrameConstantAllocator m_uiDrawCBAllocator;
    Core::Matrix m_projection;
};