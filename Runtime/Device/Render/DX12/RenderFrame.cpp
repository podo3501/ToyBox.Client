#include "pch.h"
#include "RenderFrame.h"
#include "Core/Utils/StringUtils.h"
#include "TextSystem/TextSystem.h"
#include "Resource/Material/UIMaterialResource.h"

RenderFrame::~RenderFrame() = default;
RenderFrame::RenderFrame(TextSystem& textSystem) :
    m_textSystem{ textSystem }
{}

void RenderFrame::SetFrameData(const FrameData& frameData) noexcept
{
    m_frameData = frameData;
}

struct DrawTextItem
{
    std::shared_ptr<IFontResource> fontRes;
};

void RenderFrame::DrawText(
    std::shared_ptr<IMeshResource> meshRes,
    std::shared_ptr<IFontResource> fontRes,
    std::string_view text,
    uint32_t size,
    const Core::Vector2& pos,
    const Core::Color& color)
{
    Assert(fontRes);

    auto uiItems = m_textSystem.DrawText(
        fontRes,
        Core::UTF8ToUTF32(text),
        size,
        pos,
        color);

    m_scene.AddUI(uiItems);
}

void RenderFrame::DrawSurface(
    std::shared_ptr<IMeshResource> meshRes,
    std::shared_ptr<IMaterialResource> matRes,
    const Core::Matrix& world)
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
    const Core::Matrix& world,
    const Rect* source)
{
    DrawUIItem item;
    item.mesh = meshRes;
    item.material = matRes;
    item.world = world;

    auto uiMatRes = static_cast<UIMaterialResource*>(matRes.get());
    item.uvTransform = uiMatRes->CalcUVTransform(source);

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