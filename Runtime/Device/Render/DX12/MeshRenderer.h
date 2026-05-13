#pragma once
#include <d3d12.h>
#include <wrl.h>
#include "Core/Foundation/Geometry2D.h"

struct ObjectCB;
struct FrameCB;
class CommandList;
class MeshResource;
class DescriptorAllocation;

using Microsoft::WRL::ComPtr;

class MeshRenderer
{
public:
    bool Initialize(ID3D12Device* device, const Size& screenSize);
    void BindPipeline(CommandList& cmd);
    void BindDescriptorHeap(CommandList& cmd);
    void SetFrameCB(const FrameCB& frame);
    void SetSRVHeap(ID3D12DescriptorHeap* heap) { m_srvHeap = heap; }
    void Draw(CommandList& cmd, MeshResource& mesh, DescriptorAllocation& textureSrv);
    void Resize(const Size& size);

private:
    void CreateRootSignature(ID3D12Device* device);
    void CreatePipeline(ID3D12Device* device);
    void CreateConstantBuffers(ID3D12Device* device);

private:
    void UpdateFrameCB();
    void UpdateObjectCB();

    Size m_screenSize{};

    ComPtr<ID3D12RootSignature> m_rootSignature;
    ComPtr<ID3D12PipelineState> m_pipelineState;

    ComPtr<ID3D12Resource> m_objectCB;
    ComPtr<ID3D12Resource> m_frameCB;

    ID3D12DescriptorHeap* m_srvHeap{ nullptr };
    ObjectCB* m_objectData{ nullptr };
    FrameCB* m_frameData{ nullptr };

    float m_objectAngle{ 0.f };
};