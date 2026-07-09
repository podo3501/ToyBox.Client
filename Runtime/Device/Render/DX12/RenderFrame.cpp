#include "pch.h"
#include "RenderFrame.h"
#include "Core/Utils/StringUtils.h"

RenderFrame::~RenderFrame() = default;
RenderFrame::RenderFrame() = default;

void RenderFrame::SetFrameData(const FrameData& frameData) noexcept
{
    m_frameData = frameData;
}

void RenderFrame::DrawText(
    std::shared_ptr<IFontResource> fontRes,
    std::string_view text,
    const Vector2& pos)
{
    Assert(fontRes);

    auto uiItems = m_textSystem.DrawText(
        std::move(fontRes),
        text,
        pos);

    m_scene.AddUI(uiItems);
}

void RenderFrame::DrawSurface(
    std::shared_ptr<IMeshResource> meshRes,
    std::shared_ptr<IMaterialResource> matRes,
    const Core::Math::Matrix& world)
{
    DrawItem item;
    item.mesh = meshRes;
    item.material = matRes;
    item.world = world;

    m_scene.AddSurface(item);
}

void RenderFrame::DrawUI(
    std::shared_ptr<IMeshResource> meshRes,
    std::shared_ptr<IMaterialResource> matRes,
    const Core::Math::Matrix& world,
    const Rect* source)
{
    DrawUIItem item;
    item.mesh = meshRes;
    item.material = matRes;
    item.world = world;
    if (source)
        item.source = *source;

    m_scene.AddUI(item);
}

DrawPacket RenderFrame::PrepareRenderData()
{
    m_scene.SortDraws();
    return m_scene.BuildDrawPacket();
}

void RenderFrame::Clear()
{
    m_frameData = {};
    m_scene.Clear();
}