#include "pch.h"
#include "RenderFrame.h"
#include "Core/Utils/StringUtils.h"
#include "TextSystem/TextSystem.h"
#include "Resource/Material/UIMaterialResource.h"
#include "Core/RenderData.h"

RenderFrame::~RenderFrame() = default;
RenderFrame::RenderFrame(TextSystem& textSystem) :
    m_textSystem{ textSystem }
{}

void RenderFrame::SetFrameData(const FrameData& frameData) noexcept
{
    m_frameData = frameData;
}

void RenderFrame::DrawText(
    std::shared_ptr<IFontResource> fontRes,
    std::string_view text,
    uint32_t size,
    const Core::Vector2& pos,
    const Core::Color& color)
{
    Assert(fontRes);
    if (text.empty()) return;

    DrawTextItem item;

    item.fontRes = std::move(fontRes);
    item.codePoints = Core::UTF8ToUTF32(text);
    item.fontSize = size;
    item.position = pos;
    item.color = color;

    m_pendingTexts.emplace_back(std::move(item));
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
    auto uiItems = m_textSystem.BuildDrawItems(m_pendingTexts);
    m_scene.AddUI(std::move(uiItems));
    m_pendingTexts.clear();

    m_scene.SortDraws();
    return m_scene.BuildDrawPacket();
}

void RenderFrame::Clear()
{
    m_frameData = {};
    m_pendingTexts.clear();
    m_scene.Clear();
}