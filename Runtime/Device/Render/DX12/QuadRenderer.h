#pragma once
#include <wrl/client.h>
#include <d3d12.h>
#include "Core/Foundation/Geometry2D.h"

struct CD3DX12_GPU_DESCRIPTOR_HANDLE;
struct QuadTransform;
class CommandList;
class DescriptorAllocation;
class MeshResource;

class QuadRenderer
{
public:
    ~QuadRenderer();
    QuadRenderer();
    bool Initialize(ID3D12Device* device, const Size& screenSize);
    void SetUIQuadMesh(std::shared_ptr<MeshResource> mesh);
    void SetSRVHeap(ID3D12DescriptorHeap* heap);

    void BindDescriptorHeap(CommandList& cmd);
    void BindPipeline(CommandList& cmd);
    void BindTexture(CommandList& cmd, DescriptorAllocation& srv);
    void Draw(CommandList& cmd, const Rect& dest);
    void Resize(const Size& size);

private:
    Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;

    ID3D12DescriptorHeap* m_srvHeap{ nullptr };
    UINT m_srvDescriptorSize{ 0 };
    Size m_screenSize{};

    std::shared_ptr<MeshResource> m_uiQuadMesh;
};