#include "pch.h"
#include "RenderBackend.h"
#include "DX12Core.h"
#include "SwapChainPresenter.h"
#include "CommandScheduler.h"
#include "DescriptorAllocator.h"
#include "TaskScheduler.h"
#include "TextureResource.h"
#include "ResourceLoader.h"
#include "GPUProfiler.h"
#include "MeshRenderer.h"
#include "QuadRenderer.h"
#include "TextureSystem.h"
#include "MeshSystem.h"
#include "MeshResource.h"
#include "CommandList.h"
#include "CommandUtils.h"
#include <dxgi1_6.h>
#include "TempVertex.h"

using Microsoft::WRL::ComPtr;

struct MeshBuffer
{
    ComPtr<ID3D12Resource> vertexBuffer;
};

struct QuadDrawInfo
{
    int textureIndex;
    Rect dest;
    Rect source;
};

RenderBackend::~RenderBackend() = default;
RenderBackend::RenderBackend() :
    m_core{ make_unique<DX12Core>() }
{}

bool RenderBackend::Initialize(HWND hwnd, const Size& wndSize, const RenderConfig& config)
{
    m_size = wndSize;
    ReturnIfFalse(m_core->Initialize(config.enableDebugLayer));
    auto device = m_core->GetDevice();
    auto factory = m_core->GetFactory();

    m_command = make_unique<CommandScheduler>();
    ReturnIfFalse(m_command->Initialize(device, 
        config.directQueuePoolSize, config.copyQueuePoolSize, config.computeQueuePoolSize));

    auto queue = m_command->GetCommandQueue(CommandType::Direct);
    SwapChainDesc desc{ hwnd, wndSize, config.allowTearing };
    m_swapChain = make_unique<SwapChainPresenter>();
    ReturnIfFalse(m_swapChain->Initialize(device, factory, m_command.get(), desc));

    m_srvAllocator = make_unique<DescriptorAllocator>(device);
    ReturnIfFalse(m_srvAllocator->Initialize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, config.srvDescriptorCount, true));

    m_taskScheduler = make_unique<TaskScheduler>(m_command.get());
    m_loader = make_unique<ResourceLoader>(device);
    m_profiler = make_unique<GPUProfiler>();
    ReturnIfFalse(m_profiler->Initialize(device, m_command.get()));

    m_meshSystem = make_unique<MeshSystem>(device, m_srvAllocator.get(), m_taskScheduler.get(), m_loader.get());
    m_texSystem = make_unique<TextureSystem>(device, m_srvAllocator.get(), m_taskScheduler.get(), m_loader.get());
    ReturnIfFalse(m_texSystem->Initialize());

    m_meshRenderer = make_unique<MeshRenderer>();
    ReturnIfFalse(m_meshRenderer->Initialize(device));
    m_meshRenderer->SetSRVHeap(m_srvAllocator->GetHeap());

    m_quadRenderer = make_unique<QuadRenderer>();
    ReturnIfFalse(m_quadRenderer->Initialize(device, wndSize));
    m_quadRenderer->SetSRVHeap(m_srvAllocator->GetHeap());

    auto vertices = m_quadRenderer->CreateQuadVertices();
    auto copyCmd = m_command->Begin(CommandType::Copy); // 업로드 전용 커맨드 리스트 생성
    ComPtr<ID3D12Resource> uploadBuffer;
    auto vb = m_loader->UploadVertexBuffer(
        *copyCmd,
        vertices.data(),
        static_cast<UINT>(vertices.size() * sizeof(TempVertex)),
        uploadBuffer
    );
    m_command->End({ move(uploadBuffer) }); // 업로드 완료까지 대기
    m_quadRenderer->SetVertexBuffer(vb, static_cast<UINT>(vertices.size() * sizeof(TempVertex)));
    m_meshes.push_back({ vb });

    return true;
}

void RenderBackend::BeginFrame()
{
    m_cmd = m_command->Begin(CommandType::Direct);
    if (!m_cmd) return;

    m_profiler->BeginFrame(*m_cmd);
    m_swapChain->TransitionToRenderTarget(*m_cmd);
    m_swapChain->SetRenderTarget(*m_cmd);
    Clear(*m_cmd, 0.13f, 0.13f, 0.16f, 1.0f); //눈이 적당히 덜 피곤하면서 비어있는 영역 확인 가능한 색깔.
}

void RenderBackend::EndFrame()
{
    if (!m_cmd) return;

    m_profiler->EndFrame(*m_cmd);
    m_swapChain->TransitionToPresent(*m_cmd);
    m_command->End();
    m_swapChain->Present(false);

    m_cmd = nullptr;
}

void RenderBackend::Draw(ITextureResource* texRes, const Rect& dest, const Rect* source)
{
    if (!m_cmd) return;

    auto texResource = static_cast<TextureResource*>(texRes);
    // 처음 Flush 시 한 번만 상태 전환
    static bool ready = false;
    if (!ready)
    {
        m_quadRenderer->TransitionToRenderState(*m_cmd);
        ready = true;
    }

    m_quadRenderer->BindDescriptorHeap(*m_cmd);
    m_quadRenderer->BindPipeline(*m_cmd);

    m_quadRenderer->BindTexture(*m_cmd, texResource->GetSrv());
    m_quadRenderer->Draw(*m_cmd, dest);
}

void RenderBackend::DrawMesh(IMeshResource* meshRes)
{
    if (!m_cmd) return;

    auto meshResource = static_cast<MeshResource*>(meshRes);

    m_meshRenderer->BindDescriptorHeap(*m_cmd);
    m_meshRenderer->BindPipeline(*m_cmd);

    DescriptorAllocation dummy{};
    m_meshRenderer->Draw(*m_cmd, *meshResource, dummy);
}

void RenderBackend::Resize(const Size& size)
{
    m_swapChain->Resize(m_core->GetDevice(), size);
}

void RenderBackend::Update()
{
    m_command->ReleaseCompletedResources();
    m_srvAllocator->ProcessDeferredFree(m_command->GetCompletedFences());

    m_taskScheduler->Execute();

    m_profiler->Update();
    float gpuMs = m_profiler->GetGpuFrameTimeMs();

    m_texSystem->Update(ComputeTextureBudget(gpuMs));
    m_meshSystem->Update(ComputeMeshBudget(gpuMs));
}

void RenderBackend::Clear(CommandList& cmd, float r, float g, float b, float a)
{
    auto rtv = m_swapChain->GetCurrentRTV();

    float color[4] = { r, g, b, a };
    CommandUtils::ClearRTV(cmd, rtv, color);
}

size_t RenderBackend::ComputeTextureBudget(float gpuMs)
{
    size_t baseBudget = 8 * 1024 * 1024;

    if (gpuMs > 10.0f)
        baseBudget = size_t(baseBudget * 0.7f);
    else if (gpuMs < 5.0f)
        baseBudget = size_t(baseBudget * 1.2f);

    return std::clamp<size_t>(
        baseBudget,
        4 * 1024 * 1024,
        32 * 1024 * 1024
    );
}

size_t RenderBackend::ComputeMeshBudget(float gpuMs)
{
    size_t baseBudget = 4 * 1024 * 1024; // 4MB

    if (gpuMs > 10.0f)
        baseBudget = size_t(baseBudget * 0.8f);
    else if (gpuMs < 5.0f)
        baseBudget = size_t(baseBudget * 1.25f);

    return std::clamp<size_t>(
        baseBudget,
        2 * 1024 * 1024,   // min 2MB
        16 * 1024 * 1024   // max 16MB
    );
}

ITextureSystem* RenderBackend::GetTextureSystem() 
{ 
    return m_texSystem.get(); 
}

IMeshSystem* RenderBackend::GetMeshSystem()
{
    return m_meshSystem.get();
}

//////////////////////////////////////////////////////

unique_ptr<IRenderBackend> CreateRenderBackend()
{
	return make_unique<RenderBackend>();
}
