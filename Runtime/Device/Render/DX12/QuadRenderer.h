#pragma once
#include <wrl/client.h>
#include <d3d12.h>
#include "Core/Foundation/Geometry2D.h"

struct CD3DX12_GPU_DESCRIPTOR_HANDLE;
struct ID3D12GraphicsCommandList;
struct Vertex;
struct QuadTransform;

class QuadRenderer
{
public:
    ~QuadRenderer();
    QuadRenderer();
    bool Initialize(ID3D12Device* device, const Size& screenSize);
    vector<Vertex> CreateQuadVertices() noexcept;
    void SetVertexBuffer(Microsoft::WRL::ComPtr<ID3D12Resource> vb, UINT size) noexcept;
    void SetSRVHeap(ID3D12DescriptorHeap* heap);

    void BindDescriptorHeap(ID3D12GraphicsCommandList* cmd);
    void BindPipeline(ID3D12GraphicsCommandList* cmd);
    void TransitionToRenderState(ID3D12GraphicsCommandList* cmd);
    void BindTexture(ID3D12GraphicsCommandList* cmd, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle);
    void Draw(ID3D12GraphicsCommandList* cmd, const Rect& dest);

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