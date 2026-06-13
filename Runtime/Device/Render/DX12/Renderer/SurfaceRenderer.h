#pragma once
#include "FrameUploadAllocator.h"
#include "Core/Foundation/Geometry2D.h"
#include "Core/Math/Matrix.h"
#include "GameClient/Service/Render/Desc/RenderState.h"
#include <d3d12.h>
#include <wrl.h>

struct ObjectCB;
struct MeshFrameCB;
struct MaterialCB;
struct DirectionalLightData;
struct CameraData;
struct PbrSurface;
struct PhongSurface;
class Device;
class PipelineCache;
class CommandList;
class MeshResource;
class MaterialResource;
class ShaderProvider;

using Microsoft::WRL::ComPtr;

class SurfaceRenderer
{
public:
    ~SurfaceRenderer();
    SurfaceRenderer() = delete;
    SurfaceRenderer(Device& device, PipelineCache& pipelineCache);

    bool Initialize();
    void BindCommonState(CommandList& cmd);
    void BindPipeline(CommandList& cmd, const PipelineState& pipelineState);
    void SetSRVHeap(ID3D12DescriptorHeap* heap) { m_srvHeap = heap; }
    void PrepareFrame(const DirectionalLightData& light, const CameraData& camera, uint32_t shadowSRVIndex);
    void Draw(CommandList& cmd, MeshResource& mesh, MaterialResource& material, const Core::Math::Matrix& world);
    
private:
    bool CreateRootSignature();
    void CreateDefaultPSOs();
    ID3D12PipelineState* CreatePSO(const PipelineState& pipelineState);
    void CreatePipeline(const PipelineState& pipelineState);
    ID3D12PipelineState* GetPipeline(const PipelineState& pipelineState);

    D3D12_GPU_VIRTUAL_ADDRESS UpdateObjectCB(const Core::Math::Matrix& world);
    D3D12_GPU_VIRTUAL_ADDRESS UpdateMaterialCB(MaterialResource& material);

    static constexpr UINT kCBSize = 256;
    static constexpr uint32_t kMaxObjectCount{ 1024 }; //추후에 링버퍼로 수정할 계획

    Device& m_device;
    PipelineCache& m_pipelineCache;
    std::optional<PipelineState> m_pipelineState{ nullopt };

    ComPtr<ID3D12RootSignature> m_rootSignature;
    ID3D12DescriptorHeap* m_srvHeap{ nullptr };

    FrameUploadAllocator m_objectCBAllocator;
    FrameUploadAllocator m_materialCBAllocator;
    FrameUploadAllocator m_frameCBAllocator;

    D3D12_GPU_VIRTUAL_ADDRESS m_frameCBAddress{};
};