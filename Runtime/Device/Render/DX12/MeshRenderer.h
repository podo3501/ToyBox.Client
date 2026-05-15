#pragma once
#include <d3d12.h>
#include <wrl.h>
#include "Core/Foundation/Geometry2D.h"
#include "Core/Math/Matrix.h"
#include "GameClient/Service/Render/RenderState.h"

struct ObjectCB;
struct FrameCB;
struct CameraData;
class CommandList;
class MeshResource;
class MaterialResource;
class DescriptorAllocation;

struct PSOKey
{
    FillMode fillMode{ FillMode::Solid };
    CullMode cullMode{ CullMode::Back };

    bool operator==(const PSOKey& rhs) const
    {
        return
            fillMode == rhs.fillMode &&
            cullMode == rhs.cullMode;
    }
};

struct PSOKeyHasher
{
    size_t operator()(const PSOKey& key) const
    {
        size_t h1 = std::hash<int>()((int)key.fillMode);
        size_t h2 = std::hash<int>()((int)key.cullMode);

        return h1 ^ (h2 << 1);
    }
};

using Microsoft::WRL::ComPtr;

class MeshRenderer
{
public:
    ~MeshRenderer();
    MeshRenderer() = delete;
    explicit MeshRenderer(ID3D12Device* device);

    bool Initialize(const Size& screenSize);
    void SetRasterState(const RasterState& rasterState);
    void BindPipeline(CommandList& cmd);
    void BindDescriptorHeap(CommandList& cmd);
    void SetFrameCB(const FrameCB& frame);
    void SetSRVHeap(ID3D12DescriptorHeap* heap) { m_srvHeap = heap; }
    void PrepareFrame(const CameraData& camera);
    void Draw(CommandList& cmd, MeshResource& mesh, MaterialResource& material, const Core::Math::Matrix& world);
    void Resize(const Size& size);
    
private:
    void CreateRootSignature();
    void CreateDefaultPSOs();
    void CreatePipeline(const PSOKey& key);
    void CreateConstantBuffers();
    ID3D12PipelineState* GetPipeline(const PSOKey& key);

private:
    void UpdateFrameCB();
    D3D12_GPU_VIRTUAL_ADDRESS UpdateObjectCB(const Core::Math::Matrix& world);

    static constexpr uint32_t kMaxObjectCount{ 1024 }; //추후에 링버퍼로 수정할 계획

    ID3D12Device* m_device{ nullptr };
    Size m_screenSize{};

    ComPtr<ID3D12RootSignature> m_rootSignature;
    ID3D12DescriptorHeap* m_srvHeap{ nullptr };

    RasterState m_rasterState;
    std::unordered_map<PSOKey, Microsoft::WRL::ComPtr<ID3D12PipelineState>, PSOKeyHasher> m_psoCache;

    ComPtr<ID3D12Resource> m_frameCB;
    FrameCB* m_frameData{ nullptr };

    ComPtr<ID3D12Resource> m_objectCBs[kMaxObjectCount];
    ObjectCB* m_objectDatas[kMaxObjectCount]{};
    uint32_t m_objectIndex = 0;

    float m_objectAngle{ 0.f };
};