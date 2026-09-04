#include "pch.h"
#include "UIItemBuilder.h"
#include "TextSystem/TextSystem.h"
#include "Provider/Mesh/TransientMeshProvider.h"
#include "Definition/RenderData.h"
#include "Resource/Mesh/TransientMeshResource.h"
#include "Resource/Brush/BrushResource.h"
#include "GameClient/Service/Render/Definition/View/SceneFrameData.h"

static RenderTextItem ToRenderTextItem(DrawTextItem& text)
{
    Assert(text.font);
    Rect normalized = text.bounds;
    normalized.Normalize(); // 뒤집힌 rect 방어

    RenderTextItem item;
    item.fontRes = text.font;
    item.mode = text.mode;
    item.fontSize = text.size;
    item.position = Core::Vector2{ normalized.Left(), normalized.Top() };
    item.size = Core::Vector2{ normalized.width, normalized.height };
    item.layout = text.layout;
    item.runs = std::move(text.runs);

    return item;
}

static std::vector<RenderUIItem> BuildTextItems(
    TransientMeshProvider& meshProvider,
    TextBatchBufferMap& buffers)
{
    std::vector<RenderUIItem> result;
    result.reserve(buffers.size());
    for (auto& [key, buffer] : buffers)
    {
        if (buffer.vertices.empty())
            continue;

        auto mesh = meshProvider.Create(buffer.vertices, buffer.indices);
        if (!mesh)
            continue;

        result.push_back(RenderUIItem{
            std::move(mesh),
            std::move(buffer.brush),
            Core::Matrix::Identity(),
            std::nullopt });
    }

    return result;
}

std::vector<RenderUIItem> BuildUIItems(
    ViewDrawList& draws,
    TextSystem& textSystem,
    TransientMeshProvider& meshProvider)
{
    std::vector<RenderUIItem> result;
    result.reserve(draws.ui.size() + draws.texts.size());

    for (auto& ui : draws.ui)
        result.push_back(RenderUIItem{ ui.mesh, ui.brush, ui.world, ui.source });

    if (!draws.texts.empty())
    {
        std::vector<RenderTextItem> textItems;
        textItems.reserve(draws.texts.size());

        for (auto& text : draws.texts)
        {
            if (text.runs.empty())
                continue;

            textItems.push_back(ToRenderTextItem(text));
        }

        TextBatchBufferMap buffers;
        textSystem.AppendDrawItems(textItems, buffers);
        auto textUIItems = BuildTextItems(meshProvider, buffers);

        result.insert(
            result.end(),
            std::make_move_iterator(textUIItems.begin()),
            std::make_move_iterator(textUIItems.end()));
    }

    std::sort(
        result.begin(),
        result.end(),
        [](auto& a, auto& b)
        {
            return a.sortKey < b.sortKey;
        });

    return result;
}