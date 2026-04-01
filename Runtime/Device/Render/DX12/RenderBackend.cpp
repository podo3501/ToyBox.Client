#include "pch.h"
#include "RenderBackend.h"
#include "DX12DeviceView.h"
#include "DX12Device.h"
#include "SwapChainPresenter.h"
#include "CommandScheduler.h"
#include "QuadRenderer.h"
#include "TextureLoader.h"
#include "ResourceUploader.h"
#include "ImageData.h"
#include "d3dx12.h"
#include <dxgi1_6.h>
#include "Vertex.h"

#include <wincodec.h> 

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
        m_command->FlushGPU(); //gpu가 사용중이면 중지시킨다. 
}

RenderBackend::RenderBackend() :
    m_device{ make_unique<DX12Device>() }
{}

bool RenderBackend::Initialize(HWND hwnd, const Size& wndSize, const RenderConfig& config)
{
    m_size = wndSize;
    ReturnIfFalse(m_device->Initialize(config.enableDebugLayer));
    const auto& dv = m_device->GetDeviceView();

    D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
    heapDesc.NumDescriptors = 256; // 일단 넉넉하게
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    dv.device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_srvHeap));
    m_srvDescriptorSize =
        dv.device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    
    m_swapChain = make_unique<SwapChainPresenter>(dv);
    ReturnIfFalse(m_swapChain->Initialize(hwnd, wndSize, config.allowTearing));

    m_command = make_unique<CommandScheduler>(dv);
    ReturnIfFalse(m_command->Initialize());

    m_quadRenderer = make_unique<QuadRenderer>(dv);
    ReturnIfFalse(m_quadRenderer->Initialize(wndSize));
    m_quadRenderer->SetSRVHeap(m_srvHeap.Get());

    auto uploadCmd = m_command->CreateUploadCommandList(); // 업로드 전용 커맨드 리스트 생성
    m_quadRenderer->UploadQuad(uploadCmd);
    m_command->ExecuteUploadCommandList(); // 업로드 완료까지 대기

    m_textureLoader = make_unique<TextureLoader>();
    m_uploader = make_unique<ResourceUploader>(dv);

    return true;
}

bool RenderBackend::BeginFrame()
{
    ReturnIfFalse(m_command->BeginFrame());
    PrepareRender();

    return true;
}

bool RenderBackend::EndFrame()
{
    PreparePresent();
    ReturnIfFalse(m_command->EndFrame());
    ReturnIfFalse(m_swapChain->Present(false));

    return true;
}

int RenderBackend::LoadTextureFromMemory(Core::ByteBuffer buffer)
{
    ImageData img = m_textureLoader->LoadFromMemory(move(buffer)); // CPU 디코딩

    auto uploadCmd = m_command->CreateUploadCommandList(); // 업로드용 커맨드 리스트 생성
   
    ComPtr<ID3D12Resource> uploadBuffer;
    auto texRes = m_uploader->UploadTexture(img, uploadBuffer, uploadCmd);

    CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle(m_srvHeap->GetCPUDescriptorHandleForHeapStart(),
        static_cast<UINT>(m_textures.size()), m_srvDescriptorSize);

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = texRes->GetDesc().Format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    const auto& dv = m_device->GetDeviceView();
    dv.device->CreateShaderResourceView(texRes.Get(), &srvDesc, cpuHandle);

    CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle(m_srvHeap->GetGPUDescriptorHandleForHeapStart(),
        static_cast<UINT>(m_textures.size()), m_srvDescriptorSize);

    m_textures.push_back({ texRes, uploadBuffer, gpuHandle });

    m_command->ExecuteUploadCommandList(); // 업로드 커맨드 제출 후 완료 대기

    return static_cast<int>(m_textures.size() - 1);
}

void RenderBackend::Draw(int index, const Rect& dest, const Rect* source)
{
    if (index < 0 || index >= static_cast<int>(m_textures.size())) return;

    BeginFrame();

    auto cmd = m_command->GetCommandList();
    auto& tex = m_textures[index];

    if(!m_ready)
    {
        m_quadRenderer->TransitionToRenderState(cmd);

        auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            tex.resource.Get(),
            D3D12_RESOURCE_STATE_COPY_DEST,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        cmd->ResourceBarrier(1, &barrier);

        m_ready = true;
    }

    m_quadRenderer->BindPipeline(cmd);
    m_quadRenderer->Draw(cmd, dest, tex.gpuHandle);

    EndFrame();
}

bool RenderBackend::Render()
{
    //ReturnIfFalse(BeginFrame());

    //auto cmd = m_command->GetCommandList();
    //m_quadRenderer->BindPipeline(cmd);
    //m_quadRenderer->Draw(cmd);

    //return EndFrame();

    return true;
}

void RenderBackend::Clear(float r, float g, float b, float a)
{
    auto cmd = m_command->GetCommandList();
    auto rtv = m_swapChain->GetCurrentRTV();

    float color[4] = { r, g, b, a };
    cmd->ClearRenderTargetView(rtv, color, 0, nullptr);
}

void RenderBackend::PrepareRender()
{
    auto cmdList = m_command->GetCommandList();
    m_swapChain->TransitionToRenderTarget(cmdList);
    m_swapChain->SetRenderTarget(cmdList);
}

void RenderBackend::PreparePresent()
{
    auto cmdList = m_command->GetCommandList();
    m_swapChain->TransitionToPresent(cmdList);
}

//////////////////////////////////////////////////////

unique_ptr<IRenderBackend> CreateRenderBackend()
{
	return make_unique<RenderBackend>();
}
