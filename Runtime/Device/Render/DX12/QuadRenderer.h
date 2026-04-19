#pragma once
#include <wrl/client.h>
#include <d3d12.h>
#include "Core/Foundation/Geometry2D.h"

struct CD3DX12_GPU_DESCRIPTOR_HANDLE;
struct Vertex;
struct QuadTransform;
class CommandList;
class DescriptorAllocation;

class QuadRenderer
{
public:
    ~QuadRenderer();
    QuadRenderer();
    bool Initialize(ID3D12Device* device, const Size& screenSize);
    vector<Vertex> CreateQuadVertices() noexcept;
    void SetVertexBuffer(Microsoft::WRL::ComPtr<ID3D12Resource> vb, UINT size) noexcept;
    void SetSRVHeap(ID3D12DescriptorHeap* heap);

    void BindDescriptorHeap(CommandList& cmd);
    void BindPipeline(CommandList& cmd);
    void TransitionToRenderState(CommandList& cmd);
    void BindTexture(CommandList& cmd, DescriptorAllocation& srv);
    void Draw(CommandList& cmd, const Rect& dest);

private:
    Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;

    Microsoft::WRL::ComPtr<ID3D12Resource> m_vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D12Resource> m_uploadBuffer; // 임시버퍼
    D3D12_VERTEX_BUFFER_VIEW m_vertexView{};

    ID3D12DescriptorHeap* m_srvHeap{ nullptr };
    UINT m_srvDescriptorSize{ 0 };
    Size m_screenSize{};

    Microsoft::WRL::ComPtr<ID3D12Resource> m_constantBuffer;
    QuadTransform* m_cbvData{ nullptr };
};