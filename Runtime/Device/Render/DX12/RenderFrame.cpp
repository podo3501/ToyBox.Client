#include "pch.h"
#include "RenderFrame.h"
#include "Core/Utils/StringUtils.h"
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

void RenderFrame::DrawText(
    std::shared_ptr<IFontResource> fontRes,
    TextRenderMode mode,
    std::span<const TextSpan> spans,
    uint32_t size,
    const Rect& bounds,
    const TextLayout& layout)
{
    Assert(fontRes);
    if(spans.empty()) return;

    Rect normalized = bounds;
    normalized.Normalize(); // 뒤집힌 rect 방어

    DrawTextItem item;
    item.fontRes = fontRes;
    item.mode = mode;
    item.fontSize = size;
    item.position = Core::Vector2{ normalized.Left(), normalized.Top() };
    item.size = Core::Vector2{ normalized.width, normalized.height };
    item.layout = layout;

    uint32_t lineIndex = 0;
    for (auto& span : spans)
    {
        if (span.text.empty()) continue;

        std::vector<char32_t> codepoints = Core::UTF8ToUTF32(span.text);
        size_t segStart = 0;
        for (size_t i = 0; i <= codepoints.size(); ++i)
        {
            bool isNewline = (i < codepoints.size()) && (codepoints[i] == U'\n');
            bool isEnd = (i == codepoints.size());
            if (!isNewline && !isEnd)
                continue;

            if (i > segStart) // 빈 세그먼트(연속 \n)는 run을 만들지 않음
            {
                item.runs.push_back({
                    std::vector<char32_t>(codepoints.begin() + segStart, codepoints.begin() + i),
                    span.style,
                    lineIndex
                    });
            }

            if (isNewline)
                ++lineIndex; // 내용이 있든 없든 줄 번호는 증가

            segStart = i + 1;
        }
    }

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
    std::shared_ptr<IBrushResource> brushRes,
    const Core::Matrix& world,
    const Rect* source)
{
    DrawUIItem item;
    item.mesh = meshRes;
    item.brush = brushRes;
    item.world = world;

    auto brush = static_cast<BrushResource*>(brushRes.get());
    item.uvTransform = brush->CalcUVTransform(source);

    m_scene.AddUI(item);
}

void RenderFrame::DrawEnvironment(std::shared_ptr<IEnvironmentResource> envRes)
{
    m_scene.SetEnvironment(std::move(envRes));
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