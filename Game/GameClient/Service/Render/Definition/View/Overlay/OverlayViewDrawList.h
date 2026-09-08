#pragma once
#include "GameClient/Service/Render/Resource/IResource.h"
#include "GameClient/Service/Render/Definition/Text/TextStyle.h"
#include "Core/Math/Matrix.h"
#include "Core/Foundation/Geometry2D.h"
#include <memory>
#include <vector>
#include <optional>

struct DrawUIItem
{
    std::shared_ptr<IResource> mesh;
    std::shared_ptr<IResource> brush;
    Core::Matrix world;
    std::optional<Rect> source;
};

struct DrawTextItem
{
    std::shared_ptr<IResource> font;
    TextRenderMode mode;
    uint32_t size;
    Rect bounds;
    TextLayout layout;
    std::vector<TextRun> runs;
};

struct OverlayViewDrawList
{
    std::vector<DrawUIItem> ui;
    std::vector<DrawTextItem> texts;

    bool IsEmpty() const
    {
        return ui.empty() && texts.empty();
    }

    void Clear()
    {
        ui.clear();
        texts.clear();
    }
};