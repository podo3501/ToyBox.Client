#include "pch.h"
#include "RenderBackend.h"
#include "DX12Core.h"
#include "SwapChainPresenter.h"
#include "CommandScheduler.h"
#include "DescriptorAllocator.h"
#include "QuadRenderer.h"
#include "TextureLoader.h"
#include "TextureRepository.h"
#include "ResourceUploader.h"
#include "CommandUtils.h"
#include <dxgi1_6.h>
#include "Vertex.h"

using Microsoft::WRL::ComPtr;

struct MeshEntry
{
    ComPtr<ID3D12Resource> vertexBuffer;
};

struct QuadDrawInfo
{
    int textureIndex;
    Rect dest;
    Rect source;
};

RenderBackend::~RenderBackend()
{
    if (m_command)
    {
        m_command->WaitForAllGPU(); //gpu가 사용중이면 기다린다.
    }
}

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
    ReturnIfFalse(m_command->Initialize(device, 3));

    auto queue = m_command->GetCommandQueue(CommandType::Direct);
    SwapChainDesc desc{ hwnd, wndSize, config.allowTearing };
    m_swapChain = make_unique<SwapChainPresenter>();
    ReturnIfFalse(m_swapChain->Initialize(device, factory, m_command.get(), desc));

    m_srvAllocator = make_unique<DescriptorAllocator>(device);
    ReturnIfFalse(m_srvAllocator->Initialize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 2048, true));

    m_uploader = make_unique<ResourceUploader>(device);
    m_textureRepository = make_unique<TextureRepository>(device, m_command.get(), m_srvAllocator.get(), m_uploader.get());

    m_quadRenderer = make_unique<QuadRenderer>();
    ReturnIfFalse(m_quadRenderer->Initialize(device, wndSize));
    m_quadRenderer->SetSRVHeap(m_srvAllocator->GetHeap());

    auto vertices = m_quadRenderer->CreateQuadVertices();
    auto copyCmd = m_command->Begin(CommandType::Copy); // 업로드 전용 커맨드 리스트 생성
    ComPtr<ID3D12Resource> uploadBuffer;
    auto vb = m_uploader->UploadVertexBuffer(
        copyCmd,
        vertices.data(),
        static_cast<UINT>(vertices.size() * sizeof(Vertex)),
        uploadBuffer
    );
    m_command->End({ move(uploadBuffer) }); // 업로드 완료까지 대기
    m_quadRenderer->SetVertexBuffer(vb, static_cast<UINT>(vertices.size() * sizeof(Vertex)));
    m_meshes.push_back({ vb });

    return true;
}

ID3D12GraphicsCommandList* RenderBackend::BeginFrame()
{
    auto directCmd = m_command->Begin(CommandType::Direct);
    if (!directCmd) return nullptr;

    m_swapChain->TransitionToRenderTarget(directCmd);
    m_swapChain->SetRenderTarget(directCmd);
    Clear(directCmd, 0.4f, 0.4f, 0.5f, 1.0f); //눈이 적당히 덜 피곤하면서 비어있는 영역 확인 가능한 색깔.

    return directCmd;
}

bool RenderBackend::EndFrame(ID3D12GraphicsCommandList* cmd)
{
    m_swapChain->TransitionToPresent(cmd);
    ReturnIfFalse(m_command->End());
    ReturnIfFalse(m_swapChain->Present(false));

    return true;
}

int RenderBackend::LoadTextureFromMemory(Core::ByteBuffer buffer)
{
    return m_textureRepository->LoadFromMemory(move(buffer));
}

void RenderBackend::Draw(int index, const Rect& dest, const Rect* source)
{
    m_pendingDraws.emplace_back(index, dest, source ? *source : Rect{});
}

void RenderBackend::Resize(const Size& size)
{
    m_swapChain->Resize(m_core->GetDevice(), size);
}

void RenderBackend::Update()
{
    m_command->ReleaseCompletedResources();
    FlushDraws();
}

void RenderBackend::Clear(ID3D12GraphicsCommandList* cmd, float r, float g, float b, float a)
{
    auto rtv = m_swapChain->GetCurrentRTV();

    float color[4] = { r, g, b, a };
    CommandUtils::ClearRTV(cmd, rtv, color);
}

void RenderBackend::FlushDraws()
{
    auto cmd = BeginFrame();
    if (!cmd) 
        return;

    // 처음 Flush 시 한 번만 상태 전환
    static bool ready = false;
    if (!ready)
    {
        m_quadRenderer->TransitionToRenderState(cmd);
        for (auto& draw : m_pendingDraws)
        {
            auto tex = m_textureRepository->GetTexture(draw.textureIndex);
            if (!tex) continue;

            CommandUtils::Transition(cmd, tex->resource.Get(),
                D3D12_RESOURCE_STATE_COMMON,
                D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        }
        ready = true;
    }

    m_quadRenderer->BindDescriptorHeap(cmd);
    m_quadRenderer->BindPipeline(cmd);

    int currentTex = -1;
    for (auto& draw : m_pendingDraws)
    {
        auto tex = m_textureRepository->GetTexture(draw.textureIndex);
        if (!tex) continue;

        if (draw.textureIndex != currentTex)
        {
            m_quadRenderer->BindTexture(cmd, m_srvAllocator->GetGpuHandle(tex->srvIndex));
            currentTex = draw.textureIndex;
        }

        m_quadRenderer->Draw(cmd, draw.dest);
    }
    m_pendingDraws.clear();

    EndFrame(cmd);
}


//////////////////////////////////////////////////////

unique_ptr<IRenderBackend> CreateRenderBackend()
{
	return make_unique<RenderBackend>();
}
