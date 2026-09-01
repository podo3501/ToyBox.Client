#include "pch.h"
#include "ForwardRenderPipeline.h"
#include "Graph/TaskUtils.h"
#include "Inspector/InspectorGraphBuilder.h"
#include "SwapChainPresenter.h"
#include "Factory/DescriptorFactory.h"
#include "Command/CommandList.h"
#include "Resource/Internal/ViewTargetResource.h"

ForwardRenderPipeline::~ForwardRenderPipeline() = default;
ForwardRenderPipeline::ForwardRenderPipeline(
    Device& device,
    SwapChainPresenter& swapChain,
    TaskScheduler& taskScheduler,
    DescriptorFactory& descFactory,
    ShaderLibrary& shaderLibaray,
    FontAtlasUploadGraphBuilder& fontUploadBuilder) :
    m_device{ device },
    m_swapChain{ swapChain },
    m_descFactory{ descFactory },
    m_renderers{ device, shaderLibaray },
    m_viewPool{ device, taskScheduler, descFactory },
    m_viewBuilder{ device, descFactory, m_renderers },
    m_inspectorRenderers{ device, shaderLibaray },
    //Builders
    m_fontUploadBuilder{ fontUploadBuilder },
    m_clearBuilder{ m_swapChain },
    m_shadowBuilder{
        m_renderers.GetShadowRenderer(),
        m_descFactory, m_shadowRes },
    m_compositeBuilder{
        m_renderers.GetCompositeRenderer(),
        m_swapChain },
    m_inspectorBuilder{ m_inspectorRenderers.GetInspectorImageRenderer() }
{}

bool ForwardRenderPipeline::Initialize(const Size& screenSize, const Size& shadowMapSize)
{
    ReturnIfFalse(m_shadowRes.Initialize(m_device, m_descFactory, shadowMapSize));
    ReturnIfFalse(m_renderers.Initialize());
    ReturnIfFalse(m_idAllocator.Initialize(TotalResourceIDCapacity, DynamicResourceIDCapacity));
    ReturnIfFalse(m_inspectorRenderers.Initialize(screenSize));

    m_hBackBuffer = m_idAllocator.AllocatePersistent();
    m_hShadow = m_idAllocator.AllocatePersistent();

    return true;
}

std::vector<CompiledTask> ForwardRenderPipeline::BuildFrame(const FramePacket& framePacket)
{
    m_renderers.ResetFrameResources(); // 이전 프레임에 썻던 데이터들을 초기화.
    m_graph.Reset();
    m_idAllocator.ResetTransient();

    m_graph.ImportResource(m_hBackBuffer, RGAccess::Present);
    m_graph.ImportResource(m_hShadow, RGAccess::DepthWrite);

    if (m_fontUploadBuilder.HasPendingUploads())
        m_fontUploadBuilder.Build(m_graph, m_idAllocator);

    m_clearBuilder.Build(m_graph, m_hBackBuffer);
    m_shadowBuilder.Build(m_graph, m_hShadow, framePacket.light, framePacket.shadowCasters);

    std::vector<RenderViewInfo> renderViewInfos;
    renderViewInfos.reserve(framePacket.views.size());

    std::bitset<Core::EnumSize<ViewID>> activeViews;

    FramePassContext frameCtx{ framePacket.light, m_shadowRes, m_hShadow };
    for (auto& view : framePacket.views)
    {
        activeViews.set(Core::ToIndex(view->id));

        auto size = ToSize(view->viewport.width, view->viewport.height);
        ViewTargetResource& target = m_viewPool.Acquire(view->id, m_idAllocator, size);

        renderViewInfos.push_back(m_viewBuilder.Build(m_graph, frameCtx, target, view));
    }
    m_viewPool.PruneUnused(activeViews);
    m_compositeBuilder.Build(m_graph, m_hBackBuffer, renderViewInfos);

    if (m_debugTargetID)
        m_inspectorBuilder.Build(m_graph, m_hBackBuffer, *m_debugTargetID);

    m_graph.ExportResource(m_hBackBuffer, RGAccess::Present);
    m_graph.ExportResource(m_hShadow, RGAccess::DepthWrite);

    return m_graph.Compile();
}

void ForwardRenderPipeline::Update()
{
    m_viewPool.Update();
}

void ForwardRenderPipeline::Render(
    CommandList& cmd, 
    FramePacket framePacket)
{
    auto compiledTasks = BuildFrame(framePacket); // 매 프레임 그래프 재구성

    TaskContext ctx;
    ctx.resources = std::make_shared<ResourceContext>();

    m_fontUploadBuilder.ApplyResourceBindings(*ctx.resources);
    m_viewPool.ApplyResourceBindings(*ctx.resources);
    ctx.SetResource(m_hBackBuffer, m_swapChain.GetCurrentBackbuffer());
    ctx.SetResource(m_hShadow, m_shadowRes.GetResource());

    auto& bindlessAllocator = m_descFactory.GetBindlessAllocator();
    cmd.SetBindlessHeap(bindlessAllocator.GetHeap());

    ExecuteRenderPipeline(cmd, compiledTasks, ctx);

    m_debugTargetID = m_hShadow;
}

void ForwardRenderPipeline::Resize(const Size& size)
{
    m_inspectorRenderers.SetScreenSize(size);
}

//    ctx.resources = std::make_shared<ResourceContext>(); 
// 이걸 unordered_map에서 vector나 array로 바꿔야 한다.
// ctx에 srv, dsv index 같은걸 저장할 수 있을지 생각해 보자.
// 이 index로 inspector에 넘겨줘서 화면에 나오게끔 수정.
// gpuExcute를 이름 바꾸자.  cpuExcute가 없어졌기 때문에 다른 이름이 필요.