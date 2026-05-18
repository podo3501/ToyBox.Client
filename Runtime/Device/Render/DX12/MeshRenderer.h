#pragma once
#include <d3d12.h>
#include <wrl.h>
#include "Core/Foundation/Geometry2D.h"
#include "Core/Math/Matrix.h"
#include "Core/Utils/Hash.h"
#include "GameClient/Service/Render/Repository/RenderState.h"

struct ObjectCB;
struct FrameCB;
struct MaterialCB;
struct DirectionalLightData;
struct CameraData;
struct MaterialSurface;
class CommandList;
class MeshResource;
class MaterialResource;
class DescriptorAllocation;

using Microsoft::WRL::ComPtr;

class MeshRenderer
{
public:
    ~MeshRenderer();
    MeshRenderer() = delete;
    explicit MeshRenderer(ID3D12Device* device);

    bool Initialize(const Size& screenSize);
    void BindCommonState(CommandList& cmd);
    void BindPipeline(CommandList& cmd, const PipelineState& pipelineState);
    void SetSRVHeap(ID3D12DescriptorHeap* heap) { m_srvHeap = heap; }
    void PrepareFrame(const DirectionalLightData& light, const CameraData& camera);
    void Draw(CommandList& cmd, MeshResource& mesh, MaterialResource& material, const Core::Math::Matrix& world);
    void Resize(const Size& size);
    
private:
    void CreateRootSignature();
    void CreateDefaultPSOs();
    void CreatePipeline(const PipelineState& pipelineState);
    void CreateConstantBuffers();
    ID3D12PipelineState* GetPipeline(const PipelineState& pipelineState);

    D3D12_GPU_VIRTUAL_ADDRESS UpdateObjectCB(const Core::Math::Matrix& world);
    D3D12_GPU_VIRTUAL_ADDRESS UpdateMaterialCB(const MaterialSurface& surface);

    static constexpr uint32_t kMaxObjectCount{ 1024 }; //추후에 링버퍼로 수정할 계획

    ID3D12Device* m_device{ nullptr };
    Size m_screenSize{};

    ComPtr<ID3D12RootSignature> m_rootSignature;
    ID3D12DescriptorHeap* m_srvHeap{ nullptr };

    PipelineState m_pipelineState;
    std::unordered_map<PipelineState, Microsoft::WRL::ComPtr<ID3D12PipelineState>, PipelineStateHasher> m_psoCache;

    ComPtr<ID3D12Resource> m_frameCB;
    FrameCB* m_frameData{ nullptr };

    ComPtr<ID3D12Resource> m_objectCBs[kMaxObjectCount];
    ObjectCB* m_objectDatas[kMaxObjectCount]{};

    ComPtr<ID3D12Resource> m_materialCBs[kMaxObjectCount];
    MaterialCB* m_materialDatas[kMaxObjectCount]{};

    uint32_t m_objectCBIndex{ 0 };
    uint32_t m_materialCBIndex{ 0 };

    float m_objectAngle{ 0.f };
};