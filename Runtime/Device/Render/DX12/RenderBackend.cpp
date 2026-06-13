#include "pch.h"
#include "RenderBackend.h"
#include "Core/GPUProfiler.h"
#include "SwapChainPresenter.h"
#include "Command/CommandScheduler.h"
#include "Descriptor/DescriptorAllocator.h"
#include "Descriptor/DescriptorFactory.h"
#include "Resource/ResourceLoader.h"
#include "Resource/ShadowResource.h"
#include "Renderer/Renderers.h"
#include "Scene/RenderScene.h"
#include "Graph/RenderGraph.h"
#include "Graph/RenderPass.h"
#include "Graph/TaskScheduler.h"
#include "Pipeline/ShadowGraphBuilder.h"
#include "Pipeline/OpaqueGraphBuilder.h"
#include "Pipeline/DebugSurfaceGraphBuilder.h"
#include "Pipeline/FrameEndGraphBuilder.h"
#include "Pipeline/UIGraphBuilder.h"
#include "Resource/Mesh/MeshResource.h"
#include "Resource/Mesh/MeshProvider.h"
#include "Resource/Texture/TextureProvider.h"
#include "Resource/Material/MaterialProvider.h"
#include "Resource/Material/SurfaceMaterialResource.h"
#include "Resource/Shader/ShaderProvider.h"
#include "Command/CommandList.h"
#include "Command/CommandListHelpers.h"
#include <dxgi1_6.h>

using Microsoft::WRL::ComPtr;

struct QuadDrawInfo
{
    int textureIndex;
    Rect dest;
    Rect source;
};

RenderBackend::~RenderBackend() = default;
RenderBackend::RenderBackend(const RenderConfig& config) :
    m_device{ config.enableDebugLayer },
    m_config{ config }
{}

void RenderBackend::WaitIdle()
{
    m_command->WaitIdle();
}

bool RenderBackend::Initialize(
    HWND hwnd, 
    const Size& wndSize, 
    const std::vector<ShaderRegisterDesc>& shaders)
{
    m_size = wndSize;
    m_command = make_unique<CommandScheduler>();
    ReturnIfFalse(m_command->Initialize(m_device, m_config.commandPools));

    auto queue = m_command->GetCommandQueue(CommandType::Direct);
    SwapChainDesc desc{ hwnd, wndSize, m_config.allowTearing };
    m_swapChain = make_unique<SwapChainPresenter>();
    ReturnIfFalse(m_swapChain->Initialize(m_device, m_command.get(), desc));

    m_srvAllocator = make_unique<DescriptorAllocator>();
    ReturnIfFalse(m_srvAllocator->Initialize(m_device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, m_config.descriptors.srvCount));
    m_dsvAllocator = make_unique<DescriptorAllocator>();
    ReturnIfFalse(m_dsvAllocator->Initialize(m_device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, m_config.descriptors.dsvCount));
    m_descFactory = make_unique<DescriptorFactory>(m_device, m_srvAllocator.get(), m_dsvAllocator.get());

    m_taskScheduler = make_unique<TaskScheduler>(m_command.get());
    m_loader = make_unique<ResourceLoader>(m_device);
    m_profiler = make_unique<GPUProfiler>();
    ReturnIfFalse(m_profiler->Initialize(m_device, m_command.get()));

    m_shaderProvider = make_unique<ShaderProvider>();
    ReturnIfFalse(m_shaderProvider->Initialize(shaders));
    m_texProvider = make_unique<TextureProvider>(m_device, m_descFactory.get(), m_taskScheduler.get(), m_loader.get());
    ReturnIfFalse(m_texProvider->Initialize(m_shaderProvider.get()));
    m_meshProvider = make_unique<MeshProvider>(m_descFactory.get(), m_taskScheduler.get(), m_loader.get());
    m_matProvider = make_unique<MaterialProvider>(m_texProvider.get());
    m_scene = make_unique<RenderScene>();

    m_renderers = make_unique<Renderers>(m_device, m_shaderProvider.get());
    ReturnIfFalse(m_renderers->Initialize(wndSize, m_srvAllocator->GetHeap()));

    m_shadowRes = make_unique<ShadowResource>(); //이 클래스는 framereseource 클래스중의 하나. 프레임당 render가 필요한 리소스들.
    ReturnIfFalse(m_shadowRes->Initialize(m_loader.get(), m_descFactory.get(), 2048, 2048));
    
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
    MaterialDomain domain, //?!? 나중에 matRes가 service 에서 nullptr을 채워주는 식으로 간다면 이게 필요 없어진다.
    const Core::Math::Matrix& world)
{
    if (!matRes)
    {
        switch (domain)
        {
        case MaterialDomain::Surface: matRes = m_matProvider->GetDefaultSurfaceMaterial(SurfaceType::Phong); break;
        case MaterialDomain::DebugSurface: matRes = m_matProvider->GetDefaultDebugSurfMaterial(); break;
        default: Assert(false); return; //surface가 아닌게 들어왔다.
        }
    }
    
    DrawItem item;
    item.mesh = meshRes;
    item.material = matRes;
    item.world = world;

    switch (domain)
    {
    case MaterialDomain::Surface: m_scene->AddSurface(item); break;
    case MaterialDomain::DebugSurface: m_scene->AddDebugSurface(item); break;
    }
}

void RenderBackend::DrawUI(
    std::shared_ptr<IMeshResource> meshRes,
    std::shared_ptr<IMaterialResource> matRes,
    const Core::Math::Matrix& world)
{
    DrawItem item;
    item.mesh = meshRes;
    item.material = matRes ? matRes : m_matProvider->GetDefaultUIMaterial();
    item.world = world;

    m_scene->AddUI(item);
}

void RenderBackend::Resize(const Size& size)
{
    m_renderers->SetScreenSize(size);
    m_swapChain->Resize(m_device, size);
}

void RenderBackend::Update()
{
    m_taskScheduler->Execute();

    m_profiler->Update();
    float gpuMs = m_profiler->GetGpuFrameTimeMs();

    m_texProvider->Update(ComputeTextureBudget(gpuMs));
    m_matProvider->Update();
    m_meshProvider->Update(ComputeMeshBudget(gpuMs));
}

void RenderBackend::Render()
{
    if (!BeginFrame())
        return;

    RenderGraph graph;
    auto hBb = graph.CreateRGHandle();
    auto hShadow = graph.CreateRGHandle();
    graph.ImportResource(hBb, RGAccess::Present); //backbuffer가 present에서 시작한다고 알려준다.
    graph.ImportResource(hShadow, RGAccess::DepthWrite);

    ShadowGraphBuilder shadow(m_renderers->GetShadowRenderer(), 
        m_descFactory.get(), m_shadowRes.get(), m_scene.get(), hShadow);
    OpaqueGraphBuilder opaque(m_renderers->GetSurfRenderer(), 
        m_swapChain.get(), m_shadowRes.get(), m_scene.get(), hBb, hShadow);
    DebugSurfaceGraphBuilder debugSurface(m_renderers->GetDebugSurfRenderer(), m_scene.get(), hBb);
    UIGraphBuilder ui(m_renderers->GetUIRenderer(), m_scene.get(), hBb);
    FrameEndGraphBuilder end(hBb, hShadow);

    shadow.Build(graph);
    opaque.Build(graph);
    debugSurface.Build(graph);
    ui.Build(graph);
    end.Build(graph);

    m_scene->SortDraws();

    auto compiledTasks = graph.Compile();

    TaskContext taskCtx;
    taskCtx.resources = std::make_shared<ResourceContext>();
    taskCtx.SetResource(hBb, m_swapChain->GetCurrentBackbuffer());
    taskCtx.SetResource(hShadow, m_shadowRes->GetResource());

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

ITextureProvider* RenderBackend::GetTextureProvider() 
{ 
    return m_texProvider.get(); 
}

IMeshProvider* RenderBackend::GetMeshProvider()
{
    return m_meshProvider.get();
}

IMaterialProvider* RenderBackend::GetMaterialProvider()
{
    return m_matProvider.get();
}

//////////////////////////////////////////////////////

unique_ptr<IRenderBackend> CreateRenderBackend(const RenderConfig& config)
{
	return make_unique<RenderBackend>(config);
}
