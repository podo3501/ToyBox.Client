#pragma once
#include "FrameUploadAllocator.h"
#include "PipelineCache.h"
#include "Core/Foundation/Geometry2D.h"
#include "Core/Math/Matrix.h"

struct UIFrameCB;
class CommandList;
class MeshResource;
class UIMaterialResource;
class ShaderProvider;
class FrameUploadAllocator;

class UIRenderer
{
public:
    ~UIRenderer();
    UIRenderer(ID3D12Device* device, ShaderProvider* shaderProvider);
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

    ID3D12Device* m_device{ nullptr };
    ShaderProvider* m_shaderProvider{ nullptr };
    FrameUploadAllocator m_uiFrameCBAllocator;

    Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;
    ID3D12DescriptorHeap* m_srvHeap{ nullptr };

    std::optional<PipelineState> m_pipelineState{ nullopt };
    PipelineCache m_pipelineCache;

    Core::Math::Matrix m_projection;
};