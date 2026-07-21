#pragma once
#include <memory>
#include <span>
#include "Core/Math/Matrix.h"
#include "Core/Math/Vector2.h"
#include "Core/Math/Vector4.h"
#include "Core/Foundation/Color.h"
#include "Core/Foundation/Geometry2D.h"
#include "GameClient/Service/Render/Definition/Text/TextStyle.h"

struct IFontResource;
struct IMeshResource;
struct IMaterialResource;
class TextureResource;

struct DrawItem
{
    std::shared_ptr<IMeshResource> mesh;
    std::shared_ptr<IMaterialResource> material;
    Core::Matrix world{};

    uint64_t sortKey{ 0 };
};

struct DrawUIItem : public DrawItem
{
    Core::Vector4 uvTransform{ 0.f, 0.f, 1.f, 1.f };
};

struct DrawTextItem
{
    std::shared_ptr<IFontResource> fontRes;
    std::vector<char32_t> codePoints;
    uint32_t fontSize{ 0 };
    Core::Vector2 position;
    TextStyle style;
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
    std::span<DrawItem> debugSurface;
    std::span<DrawUIItem> ui;

    DebugPacket debug;
};