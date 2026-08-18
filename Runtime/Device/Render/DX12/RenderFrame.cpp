#include "pch.h"
#include "RenderFrame.h"
#include "TextSystem/TextSystem.h"
#include "Inspector/Inspector.h"
#include "Resource/Brush/BrushResource.h"
#include "Core/RenderData.h"

RenderFrame::~RenderFrame() = default;
RenderFrame::RenderFrame(TextSystem& textSystem, Inspector& inspector) :
    m_textSystem{ textSystem },
    m_inspector{ inspector }
{}

void RenderFrame::SetFrameData(const FrameData& frameData) noexcept
{
    m_frameData = frameData;
}

ViewDrawList& RenderFrame::BeginView(const ViewContext& view)
{
    return m_scene.BeginView(view);
}

void RenderFrame::EndView()
{
    m_scene.EndView();
}

void RenderFrame::DrawText(
    std::shared_ptr<IResource> fontRes,
    TextRenderMode mode,
    uint32_t size,
    const Rect& bounds,
    const TextLayout& layout,
    std::vector<TextRun> textRuns)
{
    Assert(fontRes);
    if(textRuns.empty()) return;

    Rect normalized = bounds;
    normalized.Normalize(); // 뒤집힌 rect 방어

    DrawTextItem item;
    item.fontRes = fontRes;
    item.mode = mode;
    item.fontSize = size;
    item.position = Core::Vector2{ normalized.Left(), normalized.Top() };
    item.size = Core::Vector2{ normalized.width, normalized.height };
    item.layout = layout;
    item.runs = std::move(textRuns);

    m_pendingTexts.emplace_back(std::move(item));
}

void RenderFrame::DrawSurface(
    std::shared_ptr<IResource> meshRes,
    std::shared_ptr<IResource> matRes,
    std::optional<ShaderID> shaderOverride,
    const Core::Matrix& world)
{
    DrawItem item;
    item.mesh = std::move(meshRes);
    item.material = std::move(matRes);
    item.shaderOverride = std::move(shaderOverride);
    item.world = world;

    m_scene.AddSurface(item);
}

void RenderFrame::DrawDebugSurface(
    std::shared_ptr<IResource> meshRes,
    std::shared_ptr<IResource> matRes,
    const Core::Matrix& world)
{
    DrawDebugItem item;
    item.mesh = meshRes;
    item.material = matRes;
    item.world = world;

    m_scene.AddDebugSurface(item);
}

void RenderFrame::DrawUI(
    std::shared_ptr<IResource> meshRes,
    std::shared_ptr<IResource> brushRes,
    const Core::Matrix& world,
    const Rect* source)
{
    DrawUIItem item;
    item.mesh = meshRes;
    item.brush = brushRes;
    item.world = world;
    if (source)
        item.source = *source;

    m_scene.AddUI(item);
}

void RenderFrame::DrawEnvironment(std::shared_ptr<IResource> envRes)
{
    m_scene.SetEnvironment(envRes);
}

DrawPacket RenderFrame::PrepareRenderData()
{
    auto uiItems = m_textSystem.BuildDrawItems(m_pendingTexts);
    m_scene.AddUI(std::move(uiItems));
    m_pendingTexts.clear();

    m_scene.SortDraws();

    auto packet = m_scene.BuildDrawPacket();
    packet.debug = m_inspector.BuildPacket();

    return packet;
}

void RenderFrame::Clear()
{
    m_frameData = {};
    m_pendingTexts.clear();
    m_scene.Clear();
    m_inspector.Clear();
}