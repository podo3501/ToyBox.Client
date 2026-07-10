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

void RenderFrame::DrawText(
    std::shared_ptr<IMeshResource> meshRes,
    std::shared_ptr<IFontResource> fontRes,
    std::string_view text,
    uint32_t size,
    const Core::Math::Vector2& pos)
{
    Assert(fontRes);

    auto uiItems = m_textSystem.DrawText(
        meshRes,
        fontRes,
        text,
        size,
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