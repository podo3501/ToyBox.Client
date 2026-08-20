#pragma once
#include <memory>
#include <span>
#include "Core/Math/Matrix.h"
#include "Core/Math/Vector2.h"
#include "Core/Math/Vector4.h"
#include "Core/Foundation/Color.h"
#include "Core/Foundation/Geometry2D.h"
#include "GameClient/Service/Render/Definition/View/RenderState.h"
#include "GameClient/Service/Render/Definition/Text/TextStyle.h"

struct IMaterialResource;
struct IResource;
class TextureResource;
class BrushResource;
class EnvironmentResource;

struct RenderSurfaceItem
{
    std::shared_ptr<IResource> mesh;
    std::shared_ptr<IResource> material;
    std::optional<ShaderID> shaderOverride;
    Core::Matrix world{};

    uint64_t sortKey{ 0 };
    PipelineState pipelineState{};
};

struct RenderDebugSurfaceItem
{
    std::shared_ptr<IResource> mesh;
    std::shared_ptr<IResource> material;
    Core::Matrix world{};

    uint64_t sortKey{ 0 };
};

struct RenderUIItem
{
    std::shared_ptr<IResource> mesh;
    std::shared_ptr<IResource> brush;
    Core::Matrix world{};
    std::optional<Rect> source{ nullopt };
    
    uint64_t sortKey{ 0 };
};

struct RenderTextItem
{
    std::shared_ptr<IResource> fontRes;
    TextRenderMode mode;
    uint32_t fontSize{ 0 };
    Core::Vector2 position{};
    Core::Vector2 size{};
    TextLayout layout{};
    std::vector<TextRun> runs;
};

struct RenderInspectItem
{
    std::shared_ptr<TextureResource> texture;
};

struct DebugPacket
{
    std::vector<RenderInspectItem> images;
};

struct RenderPacket
{
    std::optional<Rect> viewport;

    std::vector<RenderSurfaceItem> surface;
    std::vector<RenderDebugSurfaceItem> debugSurface;
    std::vector<RenderUIItem> ui;
    std::shared_ptr<EnvironmentResource> environment{ nullptr }; // nullptr 가능 - 환경 없는 씬
};