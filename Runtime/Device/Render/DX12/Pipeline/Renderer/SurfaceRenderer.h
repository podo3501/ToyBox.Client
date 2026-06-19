#pragma once
#include "RendererConfig.h"
#include "FrameUploadAllocator.h"
#include "Core/Foundation/Geometry2D.h"
#include "Core/Math/Matrix.h"
#include "GameClient/Service/Render/Desc/RenderState.h"
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

using Microsoft::WRL::ComPtr;

class SurfaceRenderer
{
public:
    ~SurfaceRenderer();
    SurfaceRenderer() = delete;
    SurfaceRenderer(const SurfaceRendererConfig& config, PipelineCache& pipelineCache);

    bool Initialize(Device& device);
    void BindRootSignature(CommandList& cmd);
    void BindPipeline(CommandList& cmd, const PipelineState& pipelineState);
    void PrepareFrame(const DirectionalLightData& light, const CameraData& camera, uint32_t shadowSRVIndex);
    void Draw(CommandList& cmd, MeshResource& mesh, MaterialResource& material, const Core::Math::Matrix& world);
    
private:
    bool CreateRootSignature(Device& device);
    void CreateDefaultPSOs();
    ID3D12PipelineState* CreatePSO(const PipelineState& pipelineState);
    void CreatePipeline(const PipelineState& pipelineState);
    ID3D12PipelineState* GetPipeline(const PipelineState& pipelineState);

    D3D12_GPU_VIRTUAL_ADDRESS UpdateObjectCB(const Core::Math::Matrix& world);
    D3D12_GPU_VIRTUAL_ADDRESS UpdateMaterialCB(MaterialResource& material);

    SurfaceRendererConfig m_config;
    PipelineCache& m_pipelineCache;
    ComPtr<ID3D12RootSignature> m_rootSignature;
    std::optional<PipelineState> m_pipelineState{ nullopt };

    FrameUploadAllocator m_objectCBAllocator;
    FrameUploadAllocator m_materialCBAllocator;
    FrameUploadAllocator m_frameCBAllocator;

    D3D12_GPU_VIRTUAL_ADDRESS m_frameCBAddress{};
};