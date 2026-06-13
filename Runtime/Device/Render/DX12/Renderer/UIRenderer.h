#pragma once
#include "FrameUploadAllocator.h"
#include "Core/Foundation/Geometry2D.h"
#include "Core/Math/Matrix.h"
#include "GameClient/Service/Render/Desc/RenderState.h"

struct UIFrameCB;
class Device;
class PipelineCache;
class CommandList;
class MeshResource;
class UIMaterialResource;
class ShaderProvider;
class FrameUploadAllocator;

class UIRenderer
{
public:
    ~UIRenderer();
    UIRenderer(Device& device, PipelineCache& pipelineCache);
    bool Initialize(const Size& screenSize);
    void SetSRVHeap(ID3D12DescriptorHeap* heap) { m_srvHeap = heap; }
    void PrepareFrame();
    void BindCommonState(CommandList& cmd);
    void BindPipeline(CommandList& cmd, const PipelineState& pipelineState);
    void Draw(CommandList& cmd, MeshResource& mesh, UIMaterialResource& material, const Core::Math::Matrix& world);
    void SetScreenSize(const Size& size);

private:
    bool CreateRootSignature();
    void CreateDefaultPSOs();
    ID3D12PipelineState* CreatePSO(const PipelineState& pipelineState);
    ID3D12PipelineState* GetPipeline(const PipelineState& pipelineState);

    static constexpr UINT kMaxUI = 1024;
    static constexpr UINT kCBSize = 256;

    Device& m_device;
    PipelineCache& m_pipelineCache;
    std::optional<PipelineState> m_pipelineState{ nullopt };
    
    Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;
    ID3D12DescriptorHeap* m_srvHeap{ nullptr };

    FrameUploadAllocator m_uiFrameCBAllocator;

    Core::Math::Matrix m_projection;
};