#pragma once
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
    ShadowRenderer(Device& device, PipelineCache& pipelineCache);

    bool Initialize();
    void BindCommonState(CommandList& cmd);
    void BindPipeline(CommandList& cmd);
    void SetSRVHeap(ID3D12DescriptorHeap* heap) { m_srvHeap = heap; }
    void PrepareFrame(const DirectionalLightData& light);
    void Draw(CommandList& cmd, MeshResource& mesh, const Core::Math::Matrix& world);

private:
    bool CreateRootSignature();
    void CreateDefaultPSOs();
    void CreateConstantBuffers();
    ID3D12PipelineState* CreatePSO(const PipelineState& pipelineState);
    ID3D12PipelineState* GetPipeline(const PipelineState& pipelineState);

    D3D12_GPU_VIRTUAL_ADDRESS UpdateObjectCB(const Core::Math::Matrix& world);

    static constexpr UINT kCBSize = 256;
    static constexpr uint32_t kMaxObjectCount{ 1024 }; 

    Device& m_device;
    PipelineCache& m_pipelineCache;
    std::optional<PipelineState> m_pipelineState;

    ComPtr<ID3D12RootSignature> m_rootSignature;
    ID3D12DescriptorHeap* m_srvHeap{ nullptr };

    FrameUploadAllocator m_objectCBAllocator;
    FrameUploadAllocator m_frameCBAllocator;

    D3D12_GPU_VIRTUAL_ADDRESS m_frameCBAddress{ 0 };
};