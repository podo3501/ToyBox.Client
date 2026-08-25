#pragma once
#include "RendererConfig.h"
#include "Allocator/FrameConstantAllocator.h"
#include "Core/Foundation/Geometry2D.h"
#include "Core/Math/Matrix.h"
#include "GameClient/Service/Render/Definition/View/RenderState.h"
#include <d3d12.h>
#include <wrl.h>

struct ObjectCB;
struct MaterialCB;
struct DirectionalLightData;
struct CameraData;
class Device;
class PipelineCache;
class CommandList;
class MeshResource;
class MaterialResource;
class EnvironmentResource;

using Microsoft::WRL::ComPtr;

class SurfaceRenderer
{
public:
    ~SurfaceRenderer();
    SurfaceRenderer() = delete;
    SurfaceRenderer(const SurfaceRendererConfig& config, PipelineCache& pipelineCache);

    bool Initialize(Device& device);
    void ResetFrameResources();
    D3D12_GPU_VIRTUAL_ADDRESS PrepareFrame(
        const DirectionalLightData& light, 
        const CameraData& camera, 
        uint32_t shadowSRVIndex,
        const EnvironmentResource* envRes);
    void BeginFrame(CommandList& cmd, D3D12_GPU_VIRTUAL_ADDRESS frameCBAddress);
    void BindPipeline(CommandList& cmd, const PipelineState& pipelineState);
    void Draw(CommandList& cmd, MeshResource& mesh, MaterialResource& material, const Core::Matrix& world);
    
private:
    enum class RootSlot : uint32_t
    {
        MeshData = 0,
        FrameCB = 1,
        ObjectCB = 2,
        MaterialCB = 3
    };

    bool CreateRootSignature(Device& device);
    bool CreateDefaultPSOs();
    ID3D12PipelineState* CreatePSO(const PipelineState& pipelineState);
    ID3D12PipelineState* GetPipeline(const PipelineState& pipelineState);

    D3D12_GPU_VIRTUAL_ADDRESS UploadObjectCB(const Core::Matrix& world);
    D3D12_GPU_VIRTUAL_ADDRESS UploadMaterialCB(MaterialResource& material);

    SurfaceRendererConfig m_config;
    PipelineCache& m_pipelineCache;
    ComPtr<ID3D12RootSignature> m_rootSignature;
    ID3D12PipelineState* m_currentPSO{ nullptr };

    FrameConstantAllocator m_objectCBAllocator;
    FrameConstantAllocator m_materialCBAllocator;
    FrameConstantAllocator m_frameCBAllocator;

    D3D12_GPU_VIRTUAL_ADDRESS m_frameCBAddress{};
};