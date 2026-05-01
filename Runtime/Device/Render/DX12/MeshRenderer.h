#pragma once
#include <d3d12.h>
#include <wrl.h>

struct ObjectCB;
struct FrameCB;
class CommandList;
class MeshResource;
class DescriptorAllocation;

using Microsoft::WRL::ComPtr;

class MeshRenderer
{
public:
    bool Initialize(ID3D12Device* device);
    void BindPipeline(CommandList& cmd);
    void SetFrameCB(const FrameCB& frame);
    void Draw(CommandList& cmd, MeshResource& mesh, DescriptorAllocation& srv, const ObjectCB& obj);

private:
    void CreateRootSignature(ID3D12Device* device);
    void CreatePipeline(ID3D12Device* device);
    void CreateConstantBuffers(ID3D12Device* device);

private:
    ComPtr<ID3D12RootSignature> m_rootSignature;
    ComPtr<ID3D12PipelineState> m_pipelineState;

    ComPtr<ID3D12Resource> m_objectCB;
    ComPtr<ID3D12Resource> m_frameCB;

    ObjectCB* m_objectData{ nullptr };
    FrameCB* m_frameData{ nullptr };
};