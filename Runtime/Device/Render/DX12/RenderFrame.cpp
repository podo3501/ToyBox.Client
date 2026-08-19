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

void RenderFrame::SubmitViews(std::vector<SceneViewData> views)
{
    for (auto& view : views)
    {
        // view별 raster override는 이 view의 draw item들을 추가하는 동안만 유효해야 함
        // (Add*가 PSO를 만들 때 m_currentRasterOverride를 참조하므로, 아이템 추가 전에 세팅)
        auto rasterOverride = view.context.renderOverride.rasterPreset;

        if (view.draws.environment)
            m_scene.SetEnvironment(view.draws.environment);

        for (auto& surface : view.draws.surfaces)
        {
            m_scene.AddSurface(
                DrawItem{ 
                    surface.mesh, 
                    surface.material, 
                    surface.shaderOverride, 
                    surface.world },
                rasterOverride);
        }

        for (auto& debugSurface : view.draws.debugSurfaces)
        {
            m_scene.AddDebugSurface(
                DrawDebugItem{
                    debugSurface.mesh,
                    debugSurface.material,
                    debugSurface.world });
        }

        if (!view.draws.ui.empty())
        {
            std::vector<DrawUIItem> uiItems;
            uiItems.reserve(view.draws.ui.size());
            for (auto& ui : view.draws.ui)
            {
                uiItems.push_back(
                    DrawUIItem{
                        ui.mesh,
                        ui.brush,
                        ui.world,
                        ui.source });
            }
            m_scene.AddUI(std::move(uiItems));
        }

        if (!view.draws.texts.empty())
        {
            std::vector<DrawTextItem> textItems;
            textItems.reserve(view.draws.texts.size());
            for (auto& text : view.draws.texts)
            {
                if (text.runs.empty()) continue;
                Assert(text.font);

                Rect normalized = text.bounds;
                normalized.Normalize(); // 뒤집힌 rect 방어

                DrawTextItem item;
                item.fontRes = text.font;
                item.mode = text.mode;
                item.fontSize = text.size;
                item.position = Core::Vector2{ normalized.Left(), normalized.Top() };
                item.size = Core::Vector2{ normalized.width, normalized.height };
                item.layout = text.layout;
                item.runs = std::move(text.runs);

                textItems.push_back(std::move(item));
            }

            auto uiItems = m_textSystem.BuildDrawItems(textItems);
            m_scene.AddUI(std::move(uiItems));
        }
    }
}

DrawPacket RenderFrame::PrepareRenderData()
{
    m_scene.SortDraws();

    auto packet = m_scene.BuildDrawPacket();
    packet.debug = m_inspector.BuildPacket();

    return packet;
}

void RenderFrame::Clear()
{
    m_frameData = {};
    m_scene.Clear();
    m_inspector.Clear();
}