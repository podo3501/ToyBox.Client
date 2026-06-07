#include "pch.h"
#include "RenderBackend.h"
#include "DX12Core.h"
#include "SwapChainPresenter.h"
#include "Command/CommandScheduler.h"
#include "Descriptor/DescriptorAllocator.h"
#include "TaskScheduler.h"
#include "ResourceLoader.h"
#include "GPUProfiler.h"
#include "MaterialResource/SurfaceMaterialResource.h"
#include "Renderer/Renderers.h"
#include "TextureSystem.h"
#include "MeshSystem.h"
#include "MaterialSystem.h"
#include "ShaderSystem.h"
#include "RenderScene.h"
#include "RenderGraph.h"
#include "RenderPass.h"
#include "PrepareGraphBuilder.h"
#include "OpaqueGraphBuilder.h"
#include "GridGraphBuilder.h"
#include "PresentGraphBuilder.h"
#include "UIGraphBuilder.h"
#include "MeshResource.h"
#include "Command/CommandList.h"
#include "Helpers/CommandListHelpers.h"
#include <dxgi1_6.h>

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

void RenderBackend::WaitIdle()
{
    m_command->WaitIdle();
}

bool RenderBackend::Initialize(
    HWND hwnd, 
    const Size& wndSize, 
    const RenderConfig& config,
    const std::vector<ShaderRegisterDesc>& shaders)
{
    m_size = wndSize;
    ReturnIfFalse(m_core->Initialize(config.enableDebugLayer));
    auto device = m_core->GetDevice();
    auto factory = m_core->GetFactory();

    m_command = make_unique<CommandScheduler>();
    ReturnIfFalse(m_command->Initialize(device, 
        config.directCommandListPoolSize, 
        config.copyCommandListPoolSize,
        config.computeCommandListPoolSize));

    auto queue = m_command->GetCommandQueue(CommandType::Direct);
    SwapChainDesc desc{ hwnd, wndSize, config.allowTearing };
    m_swapChain = make_unique<SwapChainPresenter>();
    ReturnIfFalse(m_swapChain->Initialize(device, factory, m_command.get(), desc));

    m_srvAllocator = make_unique<DescriptorAllocator>(device);
    ReturnIfFalse(m_srvAllocator->Initialize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, config.srvDescriptorCount));

    m_taskScheduler = make_unique<TaskScheduler>(m_command.get());
    m_loader = make_unique<ResourceLoader>(device);
    m_profiler = make_unique<GPUProfiler>();
    ReturnIfFalse(m_profiler->Initialize(device, m_command.get()));

    m_shaderSystem = make_unique<ShaderSystem>();
    ReturnIfFalse(m_shaderSystem->Initialize(shaders));
    m_texSystem = make_unique<TextureSystem>(device, m_srvAllocator.get(), m_taskScheduler.get(), m_loader.get());
    ReturnIfFalse(m_texSystem->Initialize(m_shaderSystem.get()));
    m_meshSystem = make_unique<MeshSystem>(device, m_srvAllocator.get(), m_taskScheduler.get(), m_loader.get());
    m_matSystem = make_unique<MaterialSystem>(device, m_srvAllocator.get(), m_texSystem.get());
    m_scene = make_unique<RenderScene>();

    m_renderers = make_unique<Renderers>();
    ReturnIfFalse(m_renderers->Initialize(device, m_shaderSystem.get(), wndSize, m_srvAllocator->GetHeap()));
    
    return true;
}

void RenderBackend::SetCamera(const CameraData& camera)
{
    m_cameraData = camera;
}

void RenderBackend::SetDirectionalLight(const DirectionalLightData& light)
{
    m_lightData = light;
}

bool RenderBackend::BeginFrame()
{
    m_cmd = m_command->Begin(CommandType::Direct);
    if (m_cmd == nullptr) return false;

    return true;
}

void RenderBackend::EndFrame()
{
    assert(m_cmd);

    m_command->End();
    m_swapChain->Present(false);

    m_cmd = nullptr;
}

void RenderBackend::DrawSurface(
    std::shared_ptr<IMeshResource> meshRes,
    std::shared_ptr<IMaterialResource> matRes,
    const Core::Math::Matrix& world)
{
    if (!matRes)
        matRes = m_matSystem->GetDefaultSurfaceMaterial(SurfaceType::PBR); //?!? 나중에 Phong으로 바꾸자
    
    DrawItem item;
    item.mesh = meshRes;
    item.material = matRes;
    item.world = world;

    m_scene->AddSurface(item);
}

void RenderBackend::DrawUI(
    std::shared_ptr<IMeshResource> meshRes,
    std::shared_ptr<IMaterialResource> matRes,
    const Core::Math::Matrix& world)
{
    DrawItem item;
    item.mesh = meshRes;
    item.material = matRes ? matRes : m_matSystem->GetDefaultUIMaterial();
    item.world = world;

    m_scene->AddUI(item);
}

void RenderBackend::Resize(const Size& size)
{
    m_renderers->SetScreenSize(size);
    m_swapChain->Resize(m_core->GetDevice(), size);
}

void RenderBackend::Update()
{
    m_taskScheduler->Execute();

    m_profiler->Update();
    float gpuMs = m_profiler->GetGpuFrameTimeMs();

    m_texSystem->Update(ComputeTextureBudget(gpuMs));
    m_matSystem->Update();
    m_meshSystem->Update(ComputeMeshBudget(gpuMs));
}

void RenderBackend::Render()
{
    if (!BeginFrame())
        return;

    RenderGraph graph;
    auto hBb = graph.CreateRGHandle();
    graph.ImportResource(hBb, RGAccess::Present); //backbuffer가 present에서 시작한다고 알려준다.

    PrepareGraphBuilder prepare(m_swapChain.get(), hBb);
    OpaqueGraphBuilder opaque(m_renderers->GetMeshRenderer(), m_scene.get(), hBb);
    GridGraphBuilder grid(m_renderers->GetGridRenderer(), m_scene.get(), hBb);
    UIGraphBuilder ui(m_renderers->GetUIRenderer(), m_scene.get(), hBb);
    PresentGraphBuilder present(hBb);

    prepare.Build(graph);
    opaque.Build(graph);
    grid.Build(graph);
    ui.Build(graph);
    present.Build(graph);

    m_scene->SortDraws();

    auto compiledTasks = graph.Compile();

    TaskContext taskCtx;
    taskCtx.resources = std::make_shared<ResourceContext>();
    taskCtx.resources->Set(hBb, std::move(ComPtr<ID3D12Resource>(m_swapChain->GetCurrentBackbuffer())));

    FrameData frame;
    frame.light = m_lightData;
    frame.camera = m_cameraData;

    taskCtx.frame = frame;

    m_profiler->BeginFrame(*m_cmd);
    graph.Excute(*m_cmd, compiledTasks, taskCtx);
    m_profiler->EndFrame(*m_cmd);

    EndFrame();

    m_scene->Clear();
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

IMaterialSystem* RenderBackend::GetMaterialSystem()
{
    return m_matSystem.get();
}

//////////////////////////////////////////////////////

unique_ptr<IRenderBackend> CreateRenderBackend()
{
	return make_unique<RenderBackend>();
}
