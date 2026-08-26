#pragma once
#include <d3d12.h>
#include <wrl.h>
#include "GameClient/Service/Render/Definition/View/RenderState.h"

class Device;
class PipelineCache;
class CommandList;

using Microsoft::WRL::ComPtr;

class CompositeRenderer
{
public:
    ~CompositeRenderer();
    CompositeRenderer(PipelineCache& pipelineCache);
    bool Initialize(Device& device);

    void PrepareDraw(CommandList& cmd);
    void Draw(CommandList& cmd, UINT colorSRVIndex);

private:
    enum class RootSlot : uint32_t
    {
        CompositeData = 0
    };

    bool CreateRootSignature(Device& device);
    ID3D12PipelineState* CreatePSO(const PipelineState& pipelineState);

    PipelineCache& m_pipelineCache;
    ComPtr<ID3D12RootSignature> m_rootSignature;
    ID3D12PipelineState* m_compositePSO{ nullptr };
};