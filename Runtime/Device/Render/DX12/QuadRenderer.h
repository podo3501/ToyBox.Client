#pragma once
#include <wrl/client.h>
#include <d3d12.h>
#include "Core/Foundation/Geometry2D.h"
#include "Core/Math/Matrix.h"
#include "GameClient/Service/Render/Desc/RenderState.h"

struct CD3DX12_GPU_DESCRIPTOR_HANDLE;
struct UIFrameCB;
class CommandList;
class DescriptorAllocation;
class MeshResource;
class UIMaterialResource;
class ShaderSystem;

class QuadRenderer
{
public:
    ~QuadRenderer();
    QuadRenderer(ID3D12Device* device, ShaderSystem* shaderSystem);
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
    void CreatePipeline(const PipelineState& pipelineState);
    void CreateConstantBuffers();
    ID3D12PipelineState* GetPipeline(const PipelineState& pipelineState);

    static constexpr UINT kMaxUI = 1024;
    static constexpr UINT kCBSize = 256;

    ID3D12Device* m_device{ nullptr };
    ShaderSystem* m_shaderSystem{ nullptr };

    Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;
    ID3D12DescriptorHeap* m_srvHeap{ nullptr };

    PipelineState m_pipelineState;
    std::unordered_map<PipelineState, Microsoft::WRL::ComPtr<ID3D12PipelineState>, PipelineStateHasher> m_psoCache;

    UINT m_uiCount{ 0 };
    Core::Math::Matrix m_projection;
    Microsoft::WRL::ComPtr<ID3D12Resource> m_uiFrameCB;
    UIFrameCB* m_uiFrameData{ nullptr };
};