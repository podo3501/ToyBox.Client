#pragma once
#include <memory>
#include <span>
#include "Core/Math/Matrix.h"
#include "Core/Math/Vector2.h"
#include "Core/Math/Vector4.h"
#include "Core/Foundation/Color.h"
#include "Core/Foundation/Geometry2D.h"
#include "GameClient/Service/Render/Definition/Text/TextStyle.h"

struct IMaterialResource;
struct IResource;
class TextureResource;
class BrushResource;
class EnvironmentResource;

struct DrawItem
{
    std::shared_ptr<IResource> mesh;
    std::shared_ptr<IResource> material;
    Core::Matrix world{};

    uint64_t sortKey{ 0 };
};

struct DrawDebugItem
{
    std::shared_ptr<IResource> mesh;
    std::shared_ptr<IResource> material;
    Core::Matrix world{};

    uint64_t sortKey{ 0 };
};

struct DrawUIItem
{
    std::shared_ptr<IResource> mesh;
    std::shared_ptr<IResource> brush;
    Core::Matrix world{};
    std::optional<Rect> source{ nullopt };
    
    uint64_t sortKey{ 0 };
};

struct DrawTextRun
{
    std::vector<char32_t> codePoints;
    TextStyle style;
    uint32_t lineIndex{ 0 }; // 이 run이 몇 번째 줄에 속하는지 (절대값)
};

struct DrawTextItem
{
    std::shared_ptr<IResource> fontRes;
    TextRenderMode mode;
    uint32_t fontSize{ 0 };
    Core::Vector2 position{};
    Core::Vector2 size{};
    TextLayout layout{};
    std::vector<DrawTextRun> runs;
};

struct DrawInspectItem
{
    std::shared_ptr<TextureResource> texture;
};

struct DebugPacket
{
    std::span<DrawInspectItem> images;
};

struct DrawPacket
{
    std::span<DrawItem> surface;
    std::span<DrawDebugItem> debugSurface;
    std::span<DrawUIItem> ui;
    std::shared_ptr<EnvironmentResource> environment{ nullptr }; // nullptr 가능 - 환경 없는 씬

    DebugPacket debug;
};