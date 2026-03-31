#include "pch.h"
#include "RenderBackend.h"
#include "DX12DeviceView.h"
#include "DX12Device.h"
#include "SwapChainPresenter.h"
#include "CommandScheduler.h"
#include "QuadRenderer.h"
#include "d3dx12.h"
#include <dxgi1_6.h>

#include <wincodec.h> 

using Microsoft::WRL::ComPtr;

struct TextureEntry
{
    ComPtr<ID3D12Resource> resource;
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
    BeginFrame();

    const auto& dv = m_device->GetDeviceView();
    auto texRes = LoadFromMemory(move(buffer));

    CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle(m_srvHeap->GetCPUDescriptorHandleForHeapStart(),
        static_cast<UINT>(m_textures.size()), m_srvDescriptorSize);

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = texRes->GetDesc().Format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    dv.device->CreateShaderResourceView(texRes.Get(), &srvDesc, cpuHandle);

    CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle(m_srvHeap->GetGPUDescriptorHandleForHeapStart(),
        static_cast<UINT>(m_textures.size()), m_srvDescriptorSize);

    m_textures.push_back({ texRes, gpuHandle });

    EndFrame();

    m_command->FlushGPU();

    return static_cast<int>(m_textures.size() - 1);
}

ComPtr<ID3D12Resource> RenderBackend::LoadFromMemory(Core::ByteBuffer buffer)
{
    // WIC으로 이미지 디코딩 (PNG, JPEG 등)
    Microsoft::WRL::ComPtr<IWICImagingFactory> wicFactory;
    CoInitialize(nullptr);
    CoCreateInstance(
        CLSID_WICImagingFactory,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&wicFactory)
    );

    ComPtr<IWICStream> stream;
    wicFactory->CreateStream(&stream);
    stream->InitializeFromMemory(reinterpret_cast<BYTE*>(buffer.data()), static_cast<DWORD>(buffer.size()));

    ComPtr<IWICBitmapDecoder> decoder;
    wicFactory->CreateDecoderFromStream(stream.Get(), nullptr, WICDecodeMetadataCacheOnLoad, &decoder);

    ComPtr<IWICBitmapFrameDecode> frame;
    decoder->GetFrame(0, &frame);

    // 픽셀 포맷 변환
    ComPtr<IWICFormatConverter> converter;
    wicFactory->CreateFormatConverter(&converter);
    converter->Initialize(frame.Get(), GUID_WICPixelFormat32bppBGRA, WICBitmapDitherTypeNone, nullptr, 0.f, WICBitmapPaletteTypeCustom);

    UINT width, height;
    converter->GetSize(&width, &height);

    std::vector<BYTE> pixels(width * height * 4);
    converter->CopyPixels(nullptr, width * 4, static_cast<UINT>(pixels.size()), pixels.data());

    // 이제 DX12 텍스처 생성
    auto device = m_device->GetDeviceView().device;

    D3D12_RESOURCE_DESC texDesc{};
    texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    texDesc.Width = width;
    texDesc.Height = height;
    texDesc.DepthOrArraySize = 1;
    texDesc.MipLevels = 1;
    texDesc.SampleDesc.Count = 1;
    texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    texDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
    texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

    ComPtr<ID3D12Resource> texture;
    CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);
    device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &texDesc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&texture)
    );

    // 업로드 버퍼 생성
    UINT64 uploadSize = 0;
    device->GetCopyableFootprints(&texDesc, 0, 1, 0, nullptr, nullptr, nullptr, &uploadSize);

    ComPtr<ID3D12Resource> uploadBuffer;
    CD3DX12_HEAP_PROPERTIES uploadHeap(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadSize);
    device->CreateCommittedResource(
        &uploadHeap,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&uploadBuffer)
    );

    // 텍스처 데이터를 업로드 버퍼에 복사
    D3D12_SUBRESOURCE_DATA subresource{};
    subresource.pData = pixels.data();
    subresource.RowPitch = width * 4;
    subresource.SlicePitch = width * height * 4;

    auto cmd = m_command->GetCommandList();
    UpdateSubresources(cmd, texture.Get(), uploadBuffer.Get(), 0, 0, 1, &subresource);

    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        texture.Get(),
        D3D12_RESOURCE_STATE_COPY_DEST,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
    );
    cmd->ResourceBarrier(1, &barrier);

    return texture;
}

void RenderBackend::Draw(int index, const Rect& dest, const Rect* source)
{
    if (index < 0 || index >= static_cast<int>(m_textures.size())) return;

    BeginFrame();
    
    auto& tex = m_textures[index];
    auto cmd = m_command->GetCommandList();

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
