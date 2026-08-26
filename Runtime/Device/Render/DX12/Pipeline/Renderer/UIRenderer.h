#pragma once
#include "RendererConfig.h"
#include "Allocator/FrameConstantAllocator.h"
#include "Core/Foundation/Geometry2D.h"
#include "Core/Math/Vector4.h"
#include "Core/Math/Matrix.h"
#include "GameClient/Service/Render/Definition/View/RenderState.h"

class Device;
class PipelineCache;
class CommandList;
class MeshResource;
class BrushResource;
class FrameConstantAllocator;

class UIRenderer
{
public:
    ~UIRenderer();
    UIRenderer(const UIRendererConfig& config, PipelineCache& pipelineCache);
    bool Initialize(Device& device);
    void ResetFrameResources();

    void BeginFrame(CommandList& cmd);
    void Draw(
        CommandList& cmd, 
        MeshResource& mesh, 
        BrushResource& brush,
        const Core::Matrix& world,
        const Core::Matrix& projection,
        const std::optional<Rect>& source);

private:
    enum class RootSlot : uint32_t
    {
        ResourceIndices = 0, //vb index, ib index, tex index
        DrawCB = 1
    };

    bool CreateRootSignature(Device& device);
    ID3D12PipelineState* CreatePSO();
    D3D12_GPU_VIRTUAL_ADDRESS UploadDrawCB(
        const Core::Matrix& world, 
        const Core::Matrix& projection,
        const Core::Vector4& uvTransform);

    UIRendererConfig m_config;
    PipelineCache& m_pipelineCache;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;
    ID3D12PipelineState* m_pso{ nullptr };

    FrameConstantAllocator m_uiDrawCBAllocator;
};