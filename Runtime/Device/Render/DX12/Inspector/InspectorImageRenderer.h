#pragma once
#include "Allocator/FrameConstantAllocator.h"
#include "Core/Math/Matrix.h"
#include "Core/Foundation/Geometry2D.h"
#include "GameClient/Service/Render/Definition/View/RenderState.h"

class Device;
class PipelineCache;
class CommandList;
class TextureResource;
class MeshResource;

class InspectorImageRenderer
{
public:
    ~InspectorImageRenderer();
    InspectorImageRenderer() = delete;
    explicit InspectorImageRenderer(PipelineCache& pipelineCache) noexcept;

    bool Initialize(Device& device, const Size& screenSize);
    void PrepareFrame();
    void BeginFrame(CommandList& cmd);
    void BindPipeline(CommandList& cmd);
    void Draw(CommandList& cmd, TextureResource& texture);
    void SetScreenSize(const Size& size);

private:
    static constexpr UINT MaxImage = 10;
    static constexpr float ImageSize = 512.0f;
    static constexpr float Margin = 16.0f; //전체 창에서 여백

    enum class RootSlot : uint32_t
    {
        ResourceIndices = 0,
        DrawCB = 1
    };

    bool CreateRootSignature(Device& device);
    bool CreateDefaultPSOs();
    ID3D12PipelineState* CreatePSO(const PipelineState& pipelineState);
    ID3D12PipelineState* GetPipeline(const PipelineState& pipelineState);
    D3D12_GPU_VIRTUAL_ADDRESS UploadDrawCB();

    PipelineState m_pipelineState;
    PipelineCache& m_pipelineCache;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;
    ID3D12PipelineState* m_currentPSO{ nullptr };

    Size m_screenSize{};
    FrameConstantAllocator m_drawCBAllocator;
    Core::Matrix m_projection;
};