#pragma once
#include <memory>
#include <span>
#include "Core/Math/Matrix.h"
#include "Core/Math/Vector4.h"
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
    Core::Math::Vector4 uvTransform{ 0.f, 0.f, 1.f, 1.f };
};

struct DrawPacket
{
    std::span<DrawItem> surface;
    std::span<DrawItem> debugSurface;
    std::span<DrawUIItem> ui;
};