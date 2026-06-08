#pragma once
#include <d3d12.h>
#include <wrl.h>
#include "FrameUploadAllocator.h"
#include "PipelineCache.h"
#include "Core/Foundation/Geometry2D.h"
#include "Core/Math/Matrix.h"
#include "GameClient/Service/Render/Desc/RenderState.h"

struct ObjectCB;
struct MeshFrameCB;
struct MaterialCB;
struct DirectionalLightData;
struct CameraData;
struct PbrSurface;
struct PhongSurface;
class CommandList;
class MeshResource;
class MaterialResource;
class ShaderSystem;

using Microsoft::WRL::ComPtr;

class SurfaceRenderer
{
public:
    ~SurfaceRenderer();
    SurfaceRenderer() = delete;
    explicit SurfaceRenderer(ID3D12Device* device, ShaderSystem* shaderSystem);

    bool Initialize();
    void BindCommonState(CommandList& cmd);
    void BindPipeline(CommandList& cmd, const PipelineState& pipelineState);
    void SetSRVHeap(ID3D12DescriptorHeap* heap) { m_srvHeap = heap; }
    void PrepareFrame(const DirectionalLightData& light, const CameraData& camera);
    void Draw(CommandList& cmd, MeshResource& mesh, MaterialResource& material, const Core::Math::Matrix& world);
    
private:
    bool CreateRootSignature();
    void CreateDefaultPSOs();
    ID3D12PipelineState* CreatePSO(const PipelineState& pipelineState);
    void CreatePipeline(const PipelineState& pipelineState);
    void CreateConstantBuffers();
    ID3D12PipelineState* GetPipeline(const PipelineState& pipelineState);

    D3D12_GPU_VIRTUAL_ADDRESS UpdateObjectCB(const Core::Math::Matrix& world);
    D3D12_GPU_VIRTUAL_ADDRESS UpdateMaterialCB(MaterialResource& material);

    static constexpr UINT kCBSize = 256;
    static constexpr uint32_t kMaxObjectCount{ 1024 }; //추후에 링버퍼로 수정할 계획

    ID3D12Device* m_device{ nullptr };
    ShaderSystem* m_shaderSystem{ nullptr };

    ComPtr<ID3D12RootSignature> m_rootSignature;
    ID3D12DescriptorHeap* m_srvHeap{ nullptr };

    std::optional<PipelineState> m_pipelineState{ nullopt };
    PipelineCache m_pipelineCache;

    FrameUploadAllocator m_objectCBAllocator;
    FrameUploadAllocator m_materialCBAllocator;
    FrameUploadAllocator m_frameCBAllocator;

    D3D12_GPU_VIRTUAL_ADDRESS m_frameCBAddress{};
};