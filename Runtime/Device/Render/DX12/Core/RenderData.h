#pragma once
#include <memory>
#include <span>
#include "Core/Math/Matrix.h"
#include "Core/Math/Vector2.h"
#include "Core/Math/Vector4.h"
#include "Core/Foundation/Color.h"
#include "Core/Foundation/Geometry2D.h"

struct IFontResource;
struct IMeshResource;
struct IMaterialResource;

struct DrawItem
{
    std::shared_ptr<IMeshResource> mesh;
    std::shared_ptr<IMaterialResource> material;
    Core::Matrix world{};

    uint64_t sortKey{ 0 };
};

struct DrawTextItem
{
    std::shared_ptr<IFontResource> fontRes;
    std::vector<char32_t> codePoints;
    uint32_t fontSize{ 0 };
    Core::Vector2 position;
    Core::Color color;
};

struct DrawUIItem : public DrawItem
{
    Core::Vector4 uvTransform{ 0.f, 0.f, 1.f, 1.f };
};

struct DrawPacket
{
    std::span<DrawItem> surface;
    std::span<DrawItem> debugSurface;
    std::span<DrawUIItem> ui;
};