#include "pch.h"
#include "Renderer.h"
#include "Public/ITextureBinder.h"
#include "TextureRepository/TextureRepository.h"
#include "TextureRepository/TextureRenderTarget.h"
#include "External/DeviceResources.h"
#include "External/Imgui.h"
#include "Utils/Common.h"
#include "Config/Setting.h"

using namespace DirectX;

unique_ptr<IRenderer> CreateRenderer(HWND hwnd, int width, int height, bool bUseImgui)
{
    unique_ptr<IImgui> imgui{ nullptr };
    if (bUseImgui)
        imgui = make_unique<Imgui>(hwnd);
    else
        imgui = make_unique<NullImgui>();

    unique_ptr<Renderer> renderer = make_unique<Renderer>(hwnd, width, height, move(imgui));
    auto result = renderer->Initialize();
    if (!result)
        return nullptr;

    return move(renderer);
}

Renderer::Renderer(HWND hwnd, int width, int height, unique_ptr<IImgui>&& imgui) noexcept(false) :
    m_imgui{ move(imgui) }
{
    //WICOnceInitialize();

    m_deviceResources = make_unique<DX::DeviceResources>();
    //m_deviceResources = make_unique<DX::DeviceResources>(
    //    DXGI_FORMAT_B8G8R8A8_UNORM,
    //    DXGI_FORMAT_D32_FLOAT,
    //    2,
    //    D3D_FEATURE_LEVEL_11_0,
    //    DX::DeviceResources::c_AllowTearing); //?!? 이걸 하면 프레임 제한이 풀리지만 디버그 창에 쓰레드 종료 메세지가 계속 올라온다.

    // TODO: Provide parameters for swapchain format, depth/stencil format, and backbuffer count.
    //   Add DX::DeviceResources::c_AllowTearing to opt-in to variable rate displays.
    //   Add DX::DeviceResources::c_EnableHDR for HDR10 display.
    //   Add DX::DeviceResources::c_ReverseDepth to optimize depth buffer clears for 0 instead of 1.
    m_deviceResources->RegisterDeviceNotify(this);

    m_deviceResources->SetWindow(hwnd, width, height);
}

Renderer::~Renderer()
{
    if (m_deviceResources)
    {
        m_deviceResources->WaitForGpu();
    }
}

// Initialize the Direct3D resources required to run.
bool Renderer::Initialize()
{
    //com을 생성할때 다중쓰레드로 생성하게끔 초기화 한다. RAII이기 때문에 com을 사용할때 초기화 한다.
#ifdef __MINGW32__
    ReturnIfFailed(CoInitializeEx(nullptr, COINITBASE_MULTITHREADED))
#else
    Microsoft::WRL::Wrappers::RoInitializeWrapper initialize(RO_INIT_MULTITHREADED);
    if (FAILED(initialize)) return false;
#endif

    try
    {
        m_deviceResources->CreateDeviceResources();
        CreateDeviceDependentResources();

        m_deviceResources->CreateWindowSizeDependentResources();
        CreateWindowSizeDependentResources();
    }
    catch(CException e)
    {
        MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
        return false;
    }

    auto device = m_deviceResources->GetD3DDevice();
    auto format = m_deviceResources->GetBackBufferFormat();

    ReturnIfFalse(m_imgui->Initialize(device, m_srvDescriptors.get(), format, Ev(SrvOffset::Imgui)));
    m_batch = make_unique<ResourceUploadBatch>(device);
    m_texRepository = make_unique<TextureRepository>(
        m_deviceResources.get(), m_srvDescriptors.get(), m_batch.get(), m_spriteBatch.get());

    return true;
}

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Renderer::CreateDeviceDependentResources()
{
    auto device = m_deviceResources->GetD3DDevice();

    // Check Shader Model 6 support
    D3D12_FEATURE_DATA_SHADER_MODEL shaderModel = { D3D_SHADER_MODEL_6_0 };
    if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &shaderModel, sizeof(shaderModel)))
        || (shaderModel.HighestShaderModel < D3D_SHADER_MODEL_6_0))
    {
#ifdef _DEBUG
        OutputDebugStringA("ERROR: Shader Model 6.0 is not supported!\n");
#endif
        throw runtime_error("Shader Model 6.0 is not supported!");
    }

    // If using the DirectX Tool Kit for DX12, uncomment this line:
    m_graphicsMemory = make_unique<GraphicsMemory>(device);

    // TODO: Initialize device dependent objects here (independent of window size).
    m_srvDescriptors = make_unique<DescriptorHeap>(device, MAX_DESC);

    ResourceUploadBatch resourceUpload(device);

    resourceUpload.Begin();

    RenderTargetState rtState(m_deviceResources->GetBackBufferFormat(), m_deviceResources->GetDepthBufferFormat());

    //기본은 알파가 곱해진 형식(R*A, G*A, B*A, A)이라고 가정하는데 그러면 PNG파일에서의 알파값이 안 먹는다. 데이터에서 저렇게 뽑으면 처리할 일이 줄어서 빠르긴 하겠지.(DDS형식은 알파가 곱해져 있음)
    SpriteBatchPipelineStateDescription pd(rtState, &CommonStates::NonPremultiplied);   
    m_spriteBatch = make_unique<SpriteBatch>(device, resourceUpload, pd);

    auto uploadResourcesFinished = resourceUpload.End(m_deviceResources->GetCommandQueue());

    uploadResourcesFinished.wait();
}

// Allocate all memory resources that change on a window SizeChanged event.
void Renderer::CreateWindowSizeDependentResources()
{
    // TODO: Initialize windows-size dependent objects here.
    auto viewport = m_deviceResources->GetScreenViewport();
    m_spriteBatch->SetViewport(viewport);
}

void Renderer::OnDeviceLost()
{
    // TODO: Add Direct3D resource cleanup here.
    m_imgui->Reset();
    m_texRepository->Reset();

    m_srvDescriptors.reset();
    m_spriteBatch.reset();

    // If using the DirectX Tool Kit for DX12, uncomment this line:
    m_graphicsMemory.reset();
}

void Renderer::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}
#pragma endregion

bool Renderer::LoadTextureBinder(ITextureBinder* textureBinder)
{
    ReturnIfFalse(textureBinder);
    //com을 생성할때 다중쓰레드로 생성하게끔 초기화 한다. RAII이기 때문에 com을 사용할때 초기화 한다.
#ifdef __MINGW32__
    ReturnIfFailed(CoInitializeEx(nullptr, COINITBASE_MULTITHREADED))
#else
    Microsoft::WRL::Wrappers::RoInitializeWrapper initialize(RO_INIT_MULTITHREADED);
    if (FAILED(initialize)) return false;
#endif
    WICOnceInitialize();

    auto load = m_texRepository.get();

    m_batch->Begin();
    if(!textureBinder->LoadResources(load))
    {
        m_batch->End(m_deviceResources->GetCommandQueue());
        return false;
    }

    auto uploadResourcesFinished = m_batch->End(m_deviceResources->GetCommandQueue());
    uploadResourcesFinished.wait();

    return true;
}

#pragma region Frame Draw
// Draws the scene.
void Renderer::Draw()
{
    // Prepare the command list to render a new frame.
    m_deviceResources->Prepare();

    auto commandList = m_deviceResources->GetCommandList();
    PIXBeginEvent(commandList, PIX_COLOR_DEFAULT, L"Render");

    // TODO: Add your rendering code here.
    ID3D12DescriptorHeap* heaps[] = { m_srvDescriptors->Heap() };
    commandList->SetDescriptorHeaps(static_cast<UINT>(size(heaps)), heaps);

    // Set the viewport and scissor rect.
    auto const viewport = m_deviceResources->GetScreenViewport();
    auto const scissorRect = m_deviceResources->GetScissorRect();
    commandList->RSSetViewports(1, &viewport);
    commandList->RSSetScissorRects(1, &scissorRect);
    
    m_texRepository->DrawRenderTextures();

    Clear();

    m_spriteBatch->Begin(commandList);

    if(m_componentRenderer) 
        m_componentRenderer(m_texRepository.get());

    m_spriteBatch->End();

    //imgui들을 렌더링 한다.
    m_imgui->PrepareRender();
    m_imgui->Render(commandList);

    PIXEndEvent(commandList);

    // Show the new frame.
    PIXBeginEvent(PIX_COLOR_DEFAULT, L"Present");
    m_deviceResources->Present();

    // If using the DirectX Tool Kit for DX12, uncomment this line:
    m_graphicsMemory->Commit(m_deviceResources->GetCommandQueue());

    PIXEndEvent();
}

// Helper method to clear the back buffers.
void Renderer::Clear()
{
    auto commandList = m_deviceResources->GetCommandList();
    PIXBeginEvent(commandList, PIX_COLOR_DEFAULT, L"Clear");

    // Clear the views.
    auto const rtvDescriptor = m_deviceResources->GetRenderTargetView();
    auto const dsvDescriptor = m_deviceResources->GetDepthStencilView();

    commandList->OMSetRenderTargets(1, &rtvDescriptor, FALSE, &dsvDescriptor);
    commandList->ClearRenderTargetView(rtvDescriptor, Colors::CornflowerBlue, 0, nullptr);
    commandList->ClearDepthStencilView(dsvDescriptor, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    // Set the viewport and scissor rect.
    auto const viewport = m_deviceResources->GetScreenViewport();
    auto const scissorRect = m_deviceResources->GetScissorRect();
    commandList->RSSetViewports(1, &viewport);
    commandList->RSSetScissorRects(1, &scissorRect);

    PIXEndEvent(commandList);
}
#pragma endregion

#pragma region Message Handlers
// Message handlers
void Renderer::OnActivated()
{
    // TODO: Renderer is becoming active window.
}

void Renderer::OnDeactivated()
{
    // TODO: Renderer is becoming background window.
}

void Renderer::OnSuspending()
{
    // TODO: Renderer is being power-suspended (or minimized).
}

void Renderer::OnResuming()
{
    // TODO: Renderer is being power-resumed (or returning from minimize).
}

void Renderer::OnWindowMoved()
{
    auto const r = m_deviceResources->GetOutputSize();
    m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Renderer::OnDisplayChange()
{
    m_deviceResources->UpdateColorSpace();
}

void Renderer::OnWindowSizeChanged(int width, int height)
{
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;

    CreateWindowSizeDependentResources();

    // TODO: Renderer window is being resized.
}
#pragma endregion

ITextureController* Renderer::GetTextureController() const noexcept { return m_texRepository.get(); }

void Renderer::SetComponentRenderer(function<void(ITextureRender*)> rendererFn) noexcept
{
    m_componentRenderer = rendererFn;
}

void Renderer::AddImguiComponent(IImguiComponent* comp) { m_imgui->AddComponent(comp); }
void Renderer::RemoveImguiComponent(IImguiComponent* comp) noexcept { m_imgui->RemoveComponent(comp); }