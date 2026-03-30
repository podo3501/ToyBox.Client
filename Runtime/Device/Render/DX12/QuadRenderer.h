#pragma once
#include "DX12Core.h"
#include <wrl/client.h>
#include <d3d12.h>

class QuadRenderer
{
public:
    ~QuadRenderer();
    QuadRenderer(DX12Core core);
    bool Initialize();

    void BindPipeline(ID3D12GraphicsCommandList* cmd);
    void Draw(ID3D12GraphicsCommandList* cmd);

private:
    DX12Core m_core{};

    Microsoft::WRL::ComPtr<ID3D12Resource> m_vertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW m_vertexView;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;
};