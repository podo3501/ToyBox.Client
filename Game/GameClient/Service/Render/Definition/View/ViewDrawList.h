#pragma once
#include "RenderState.h"
#include "GameClient/Service/Render/Resource/IResource.h"
#include "GameClient/Service/Render/Definition/Text/TextStyle.h"
#include "Core/Math/Matrix.h"
#include "Core/Foundation/Geometry2D.h"
#include <memory>

struct SurfaceDrawItem
{
    std::shared_ptr<IResource> mesh;
    std::shared_ptr<IResource> material;
    std::optional<ShaderID> shaderOverride;
    Core::Matrix world;
};

struct DebugSurfaceDrawItem
{
    std::shared_ptr<IResource> mesh;
    std::shared_ptr<IResource> material;
    Core::Matrix world;
};

struct UIDrawItem
{
    std::shared_ptr<IResource> mesh;
    std::shared_ptr<IResource> brush;
    Core::Matrix world;
    std::optional<Rect> source;
};

struct TextDrawItem
{
    std::shared_ptr<IResource> font;
    TextRenderMode mode;
    uint32_t size;
    Rect bounds;
    TextLayout layout;
    std::vector<TextRun> runs;
};

struct ViewDrawList
{
    std::shared_ptr<IResource> environment;
    std::vector<SurfaceDrawItem> surfaces;
    std::vector<DebugSurfaceDrawItem> debugSurfaces;
    std::vector<UIDrawItem> ui;
    std::vector<TextDrawItem> texts;

    bool IsEmpty() const
    {
        return !environment &&
            surfaces.empty() &&
            debugSurfaces.empty() &&
            ui.empty() &&
            texts.empty();
    }

    void Clear()
    {
        environment.reset();
        surfaces.clear();
        debugSurfaces.clear();
        ui.clear();
        texts.clear();
    }
};