#include "pch.h"
#include "DeviceResources.h"
#include "Common.h"
#include "DebugHelper.h"
#include "GameClient/Service/Render/RenderConfig.h"
#include <d3dcompiler.h>

using namespace DirectX;
using Microsoft::WRL::ComPtr;

DeviceResources::~DeviceResources()
{
    if (m_sync.eventHandle)
        CloseHandle(m_sync.eventHandle);
}
DeviceResources::DeviceResources() = default;

bool DeviceResources::Initialize(HWND hwnd, const Size& size, const RenderConfig& config)
{
#if defined(_DEBUG)
    if (config.enableDebugLayer)
        DebugHelper::EnableDebugLayer();
#endif

    m_tearing = config.allowTearing;
    m_size = size;

    ReturnIfFalse(CreateFactory(config.enableDebugLayer));
    CheckTearingSupport(m_tearing);
    ReturnIfFalse(CreateDevice());
    ReturnIfFalse(CreateCommandQueue());
    ReturnIfFalse(CreateSwapChain(hwnd, m_size, m_tearing));

    ReturnIfFalse(CreateRTV());
    ReturnIfFalse(CreateCommandObjects());
    ReturnIfFalse(CreateFence());

    CreateQuadResources();

    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

    return true;
}

bool DeviceResources::BeginFrame()
{
    if (FAILED(m_command.allocator->Reset()))
        return false;

    if (FAILED(m_command.list->Reset(m_command.allocator.Get(), nullptr)))
        return false;

    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_rtv.buffers[m_frameIndex].Get(),
        D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATE_RENDER_TARGET);

    m_command.list->ResourceBarrier(1, &barrier);

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
        m_rtv.heap->GetCPUDescriptorHandleForHeapStart(),
        m_frameIndex,
        m_rtv.descriptorSize);

    m_command.list->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

    D3D12_VIEWPORT viewport{};
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = static_cast<float>(m_size.width);
    viewport.Height = static_cast<float>(m_size.height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    D3D12_RECT scissor{};
    scissor.left = 0;
    scissor.top = 0;
    scissor.right = m_size.width;
    scissor.bottom = m_size.height;

    m_command.list->RSSetViewports(1, &viewport);
    m_command.list->RSSetScissorRects(1, &scissor);

    return true;
}

bool DeviceResources::EndFrame()
{
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_rtv.buffers[m_frameIndex].Get(),
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PRESENT);

    m_command.list->ResourceBarrier(1, &barrier);

    if (FAILED(m_command.list->Close()))
        return false;

    ID3D12CommandList* lists[] = { m_command.list.Get() };
    m_commandQueue->ExecuteCommandLists(1, lists);

    return true;
}

bool DeviceResources::Present(bool vsync)
{
    UINT syncInterval = vsync ? 1 : 0;
    UINT flags = (!vsync && m_tearing) ? DXGI_PRESENT_ALLOW_TEARING : 0;

    if (FAILED(m_swapChain->Present(syncInterval, flags)))
        return false;

    ReturnIfFalse(FlushGPU());
    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

    return true;
}

bool DeviceResources::Resize(const Size& size) //WM_SIZE: renderer->Resize(width, height);
{
    if (size.width == 0 || size.height == 0) return false;
    if (m_size == size) return true;

    ReturnIfFalse(FlushGPU()); // GPU 작업 끝날 때까지 대기

    for (UINT i = 0; i < FrameCount; ++i)
        m_rtv.buffers[i].Reset(); //기존 RTV 리소스 해제

    DXGI_SWAP_CHAIN_DESC desc{}; //SwapChain Resize
    if (FAILED(m_swapChain->GetDesc(&desc)))
        return false;

    UINT flags = desc.Flags;

    if (FAILED(m_swapChain->ResizeBuffers(
        FrameCount,
        size.width,
        size.height,
        desc.BufferDesc.Format,
        flags)))
        return false;

    m_size = size;
    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

    CD3DX12_CPU_DESCRIPTOR_HANDLE handle(
        m_rtv.heap->GetCPUDescriptorHandleForHeapStart()); //RTV 재생성

    for (UINT i = 0; i < FrameCount; ++i)
    {
        if (FAILED(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_rtv.buffers[i]))))
            return false;

        m_device->CreateRenderTargetView(
            m_rtv.buffers[i].Get(),
            nullptr,
            handle);

        handle.Offset(1, m_rtv.descriptorSize);
    }

    return true;
}

void DeviceResources::BindQuadPipeline(ID3D12GraphicsCommandList* cmd)
{
    cmd->SetGraphicsRootSignature(m_rootSignature.Get());
    cmd->SetPipelineState(m_pso.Get());
}

void DeviceResources::DrawQuad(ID3D12GraphicsCommandList* cmd)
{
    cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmd->IASetVertexBuffers(0, 1, &m_quadVBView);

    cmd->DrawInstanced(6, 1, 0, 0);
}

void DeviceResources::CheckTearingSupport(bool& allowTearing)
{
    if (!allowTearing) return;
    BOOL supported = FALSE;
    ComPtr<IDXGIFactory5> factory5;
    if (SUCCEEDED(m_dxgiFactory.As(&factory5))) 
    {
        if (FAILED(factory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING,
            &supported, sizeof(supported))) || !supported)
            allowTearing = false;
    }
}

bool DeviceResources::CreateFactory(bool enableDebug)
{
    UINT flags = enableDebug ? DXGI_CREATE_FACTORY_DEBUG : 0;
    return SUCCEEDED(CreateDXGIFactory2(flags, IID_PPV_ARGS(&m_dxgiFactory)));
}

bool DeviceResources::CreateDevice()
{
    ComPtr<IDXGIAdapter1> adapter;

    for (UINT i = 0; m_dxgiFactory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i)
    {
        DXGI_ADAPTER_DESC1 desc;
        adapter->GetDesc1(&desc);

        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            continue;

        if (SUCCEEDED(D3D12CreateDevice(adapter.Get(),
            D3D_FEATURE_LEVEL_12_1,
            __uuidof(ID3D12Device),
            nullptr)))
        {
            break;
        }

        adapter.Reset();
    }

    if (!adapter)
    {
        ComPtr<IDXGIAdapter> warpAdapter;
        if (FAILED(m_dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter))))
            return false;

        return SUCCEEDED(D3D12CreateDevice(
            warpAdapter.Get(),
            D3D_FEATURE_LEVEL_12_1,
            IID_PPV_ARGS(&m_device)));
    }

    return SUCCEEDED(D3D12CreateDevice(
        adapter.Get(),
        D3D_FEATURE_LEVEL_12_1,
        IID_PPV_ARGS(&m_device)));
}

bool DeviceResources::CreateCommandQueue()
{
    D3D12_COMMAND_QUEUE_DESC desc{};
    desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    return SUCCEEDED(
        m_device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_commandQueue))
    );
}

bool DeviceResources::CreateSwapChain(HWND hwnd, const Size& wndSize, bool allowTearing)
{
    DXGI_SWAP_CHAIN_DESC1 scDesc{};
    scDesc.BufferCount = 2;
    scDesc.Width = wndSize.width;
    scDesc.Height = wndSize.height;
    scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    scDesc.SampleDesc.Count = 1;
    scDesc.Scaling = DXGI_SCALING_STRETCH;
    scDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
    scDesc.Flags = allowTearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

    ComPtr<IDXGISwapChain1> swapChain1;

    if (FAILED(m_dxgiFactory->CreateSwapChainForHwnd(
        m_commandQueue.Get(),
        hwnd,
        &scDesc,
        nullptr,
        nullptr,
        &swapChain1)))
        return false;

    m_dxgiFactory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);

    return SUCCEEDED(swapChain1.As(&m_swapChain));
}

bool DeviceResources::CreateRTV()
{
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
    heapDesc.NumDescriptors = FrameCount;
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    if (FAILED(m_device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_rtv.heap))))
        return false;

    m_rtv.descriptorSize =
        m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    CD3DX12_CPU_DESCRIPTOR_HANDLE handle(
        m_rtv.heap->GetCPUDescriptorHandleForHeapStart());

    for (UINT i = 0; i < FrameCount; ++i)
    {
        if (FAILED(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_rtv.buffers[i]))))
            return false;

        m_device->CreateRenderTargetView(
            m_rtv.buffers[i].Get(),
            nullptr,
            handle);

        handle.Offset(1, m_rtv.descriptorSize);
    }

    return true;
}

bool DeviceResources::CreateCommandObjects()
{
    if (FAILED(m_device->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        IID_PPV_ARGS(&m_command.allocator))))
        return false;

    if (FAILED(m_device->CreateCommandList(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        m_command.allocator.Get(),
        nullptr,
        IID_PPV_ARGS(&m_command.list))))
        return false;

    // 처음엔 닫아야 함
    m_command.list->Close();

    return true;
}

bool DeviceResources::CreateFence()
{
    if (FAILED(m_device->CreateFence(
        0,
        D3D12_FENCE_FLAG_NONE,
        IID_PPV_ARGS(&m_sync.fence))))
        return false;

    m_sync.value = 1;

    m_sync.eventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (!m_sync.eventHandle)
        return false;

    return true;
}

bool DeviceResources::FlushGPU()
{
    const UINT64 fenceToWait = m_sync.value;

    if (FAILED(m_commandQueue->Signal(m_sync.fence.Get(), fenceToWait)))
        return false;

    m_sync.value++;

    if (m_sync.fence->GetCompletedValue() < fenceToWait)
    {
        if (FAILED(m_sync.fence->SetEventOnCompletion(fenceToWait, m_sync.eventHandle)))
            return false;

        WaitForSingleObject(m_sync.eventHandle, INFINITE);
    }

    return true;
}

void DeviceResources::CreateQuadResources()
{
    Vertex quad[] =
    {
        { -0.5f, -0.5f, 0, 1,0,0,1 },
        { -0.5f,  0.5f, 0, 0,1,0,1 },
        {  0.5f, -0.5f, 0, 0,0,1,1 },

        {  0.5f, -0.5f, 0, 0,0,1,1 },
        { -0.5f,  0.5f, 0, 0,1,0,1 },
        {  0.5f,  0.5f, 0, 1,1,1,1 },
    };

    UINT vbSize = sizeof(quad);

    CD3DX12_HEAP_PROPERTIES heap(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(vbSize);

    m_device->CreateCommittedResource( 
        &heap,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_quadVB)
    ); //VertexBuffer (Upload Heap)

    void* data;
    m_quadVB->Map(0, nullptr, &data);
    memcpy(data, quad, vbSize);
    m_quadVB->Unmap(0, nullptr);

    m_quadVBView.BufferLocation = m_quadVB->GetGPUVirtualAddress();
    m_quadVBView.SizeInBytes = vbSize;
    m_quadVBView.StrideInBytes = sizeof(Vertex);

    CD3DX12_ROOT_SIGNATURE_DESC rsDesc;
    rsDesc.Init(0, nullptr, 0, nullptr,
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT); // RootSignature(비어있음)

    ComPtr<ID3DBlob> sig;
    ComPtr<ID3DBlob> err;

    D3D12SerializeRootSignature(&rsDesc, D3D_ROOT_SIGNATURE_VERSION_1, &sig, &err);

    m_device->CreateRootSignature(
        0,
        sig->GetBufferPointer(),
        sig->GetBufferSize(),
        IID_PPV_ARGS(&m_rootSignature)
    );

    ComPtr<ID3DBlob> vs; //Shader(컴파일했다고 가정)
    ComPtr<ID3DBlob> ps;
    
    wstring shaderFile = L"D:\\ProgrammingStudy\\ToyBox\\Runtime\\Device\\Render\\DX12\\Quad.hlsl";
    D3DCompileFromFile(shaderFile.c_str(), nullptr, nullptr, "VSMain", "vs_5_0", 0, 0, &vs, nullptr);
    D3DCompileFromFile(shaderFile.c_str(), nullptr, nullptr, "PSMain", "ps_5_0", 0, 0, &ps, nullptr);

    D3D12_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },

        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    }; // Input Layout

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.InputLayout = { layout, _countof(layout) };
    psoDesc.pRootSignature = m_rootSignature.Get();
    psoDesc.VS = { vs->GetBufferPointer(), vs->GetBufferSize() };
    psoDesc.PS = { ps->GetBufferPointer(), ps->GetBufferSize() };

    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

    psoDesc.DepthStencilState.DepthEnable = FALSE;
    psoDesc.DepthStencilState.StencilEnable = FALSE;

    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.SampleDesc.Count = 1;

    m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pso));
}

ID3D12Device* DeviceResources::GetDevice() const { return m_device.Get(); }
ID3D12CommandQueue* DeviceResources::GetCommandQueue() const { return m_commandQueue.Get(); }
IDXGISwapChain4* DeviceResources::GetSwapChain() const { return m_swapChain.Get(); }
ID3D12GraphicsCommandList* DeviceResources::GetCommandList() const { return m_command.list.Get(); }
D3D12_CPU_DESCRIPTOR_HANDLE DeviceResources::GetCurrentRTV() const
{
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(
        m_rtv.heap->GetCPUDescriptorHandleForHeapStart(),
        m_frameIndex,
        m_rtv.descriptorSize);
}