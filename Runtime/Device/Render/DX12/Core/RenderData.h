#pragma once
#include <memory>
#include <span>
#include "Core/Math/Matrix.h"
#include "Core/Foundation/Geometry2D.h"

struct IMeshResource;
struct IMaterialResource;

struct DrawItem
{
    std::shared_ptr<IMeshResource> mesh;
    std::shared_ptr<IMaterialResource> material;
    Core::Math::Matrix world;

    uint64_t sortKey{ 0 };
};

struct DrawUIItem : public DrawItem
{
    std::optional<Rect> source{ std::nullopt };
};

struct DrawPacket
{
    std::span<DrawItem> surface;
    std::span<DrawItem> debugSurface;
    std::span<DrawUIItem> ui;
};