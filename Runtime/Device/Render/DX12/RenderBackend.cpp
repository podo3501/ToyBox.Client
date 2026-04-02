#include "pch.h"
#include "RenderBackend.h"
#include "DX12DeviceView.h"
#include "DX12Core.h"
#include "SwapChainPresenter.h"
#include "CommandScheduler.h"
#include "QuadRenderer.h"
#include "TextureLoader.h"
#include "ResourceUploader.h"
#include "ImageData.h"
#include "CommandUtils.h"
#include <dxgi1_6.h>
#include "Vertex.h"

using Microsoft::WRL::ComPtr;

struct TextureEntry
{
    ComPtr<ID3D12Resource> resource;
    ComPtr<ID3D12Resource> uploadBuffer;
    CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle;
};

RenderBackend::~RenderBackend()
{
    if(m_command)
        m_command->Flush(); //gpu가 사용중이면 중지시킨다. 
}

RenderBackend::RenderBackend() :
    m_core{ make_unique<DX12Core>() }
{}

bool RenderBackend::Initialize(HWND hwnd, const Size& wndSize, const RenderConfig& config)
{
    m_size = wndSize;
    ReturnIfFalse(m_core->Initialize(config.enableDebugLayer));
    const auto& dv = m_core->GetDeviceView();
    auto device = m_core->GetDevice();
    auto factory = m_core->GetFactory();

    D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
    heapDesc.NumDescriptors = 256; // 일단 넉넉하게
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    dv.device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_srvHeap));
    m_srvDescriptorSize =
        dv.device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    
    m_command = make_unique<CommandScheduler>();
    ReturnIfFalse(m_command->Initialize(device));

    auto queue = m_command->GetCommandQueue(CommandType::Direct);
    SwapChainDesc desc{ hwnd, wndSize, config.allowTearing };
    m_swapChain = make_unique<SwapChainPresenter>();
    ReturnIfFalse(m_swapChain->Initialize(device, factory, queue, desc));

    m_quadRenderer = make_unique<QuadRenderer>(dv);
    ReturnIfFalse(m_quadRenderer->Initialize(wndSize));
    m_quadRenderer->SetSRVHeap(m_srvHeap.Get());

    auto copyCmd = m_command->Begin(CommandType::Copy); // 업로드 전용 커맨드 리스트 생성
    m_quadRenderer->UploadQuad(copyCmd);
    m_command->End(); // 업로드 완료까지 대기

    m_textureLoader = make_unique<TextureLoader>();
    m_uploader = make_unique<ResourceUploader>(dv);

    return true;
}

ID3D12GraphicsCommandList* RenderBackend::BeginFrame()
{
    auto directCmd = m_command->Begin(CommandType::Direct);
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
    ImageData img = m_textureLoader->LoadFromMemory(move(buffer)); // CPU 디코딩

    auto uploadCmd = m_command->Begin(CommandType::Copy); // 업로드용 커맨드 리스트 생성
   
    ComPtr<ID3D12Resource> uploadBuffer;
    auto texRes = m_uploader->UploadTexture(img, uploadBuffer, uploadCmd);

    CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle(m_srvHeap->GetCPUDescriptorHandleForHeapStart(),
        static_cast<UINT>(m_textures.size()), m_srvDescriptorSize);

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = texRes->GetDesc().Format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    const auto& dv = m_core->GetDeviceView();
    dv.device->CreateShaderResourceView(texRes.Get(), &srvDesc, cpuHandle);

    CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle(m_srvHeap->GetGPUDescriptorHandleForHeapStart(),
        static_cast<UINT>(m_textures.size()), m_srvDescriptorSize);

    m_textures.push_back({ texRes, uploadBuffer, gpuHandle });

    m_command->End(); // 업로드 커맨드 제출 후 완료 대기

    return static_cast<int>(m_textures.size() - 1);
}

void RenderBackend::Draw(int index, const Rect& dest, const Rect* source)
{
    if (index < 0 || index >= static_cast<int>(m_textures.size())) return;

    auto cmd = BeginFrame();

    auto& tex = m_textures[index];
    if(!m_ready)
    {
        m_quadRenderer->TransitionToRenderState(cmd);

        CommandUtils::Transition(cmd, tex.resource.Get(),
            D3D12_RESOURCE_STATE_COMMON,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

        m_ready = true;
    }

    m_quadRenderer->BindPipeline(cmd);
    m_quadRenderer->Draw(cmd, dest, tex.gpuHandle);

    EndFrame(cmd);
}

void RenderBackend::Clear(ID3D12GraphicsCommandList* cmd, float r, float g, float b, float a)
{
    auto rtv = m_swapChain->GetCurrentRTV();

    float color[4] = { r, g, b, a };
    CommandUtils::ClearRTV(cmd, rtv, color);
}

//////////////////////////////////////////////////////

unique_ptr<IRenderBackend> CreateRenderBackend()
{
	return make_unique<RenderBackend>();
}
