#pragma once
#include "PipelineCache.h"
#include "FrameUploadAllocator.h"
#include "Core/Math/Matrix.h"

struct CameraData;
class CommandList;
class ShaderSystem;
class MeshResource;

using Microsoft::WRL::ComPtr;

class GridRenderer
{
public:
    ~GridRenderer();
    GridRenderer() = delete;
    explicit GridRenderer(ID3D12Device* device, ShaderSystem* shaderSystem);

    bool Initialize();
    void BindCommonState(CommandList& cmd);
    void BindPipeline(CommandList& cmd, const PipelineState& pipelineState);
    void SetSRVHeap(ID3D12DescriptorHeap* heap) { m_srvHeap = heap; }
    void PrepareFrame(const CameraData& camera);
    void Draw(CommandList& cmd, MeshResource& mesh, const Core::Math::Matrix& world);

private:
    bool CreateRootSignature();
    void CreateDefaultPSOs();
    ID3D12PipelineState* CreatePSO(const PipelineState& pipelineState);
    ID3D12PipelineState* GetPipeline(const PipelineState& pipelineState);
    void CreateConstantBuffers();

    D3D12_GPU_VIRTUAL_ADDRESS UpdateObjectCB(const Core::Math::Matrix& world);

    static constexpr UINT kCBSize = 256;
    static constexpr uint32_t kMaxObjectCount = 1024;

    ID3D12Device* m_device{ nullptr };
    ShaderSystem* m_shaderSystem{ nullptr };

    ComPtr<ID3D12RootSignature> m_rootSignature;

    ID3D12DescriptorHeap* m_srvHeap{ nullptr };

    PipelineState m_pipelineState;
    PipelineCache m_pipelineCache;

    FrameUploadAllocator m_objectCBAllocator;
    FrameUploadAllocator m_frameCBAllocator;

    D3D12_GPU_VIRTUAL_ADDRESS m_frameCBAddress{};
};