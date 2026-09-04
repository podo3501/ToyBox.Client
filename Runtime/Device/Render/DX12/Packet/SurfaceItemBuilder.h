#pragma once

struct RenderSurfaceItem;
struct DrawSurfaceItem;
enum class RasterPreset;

std::vector<RenderSurfaceItem> BuildSurfaceItems(
    std::vector<DrawSurfaceItem>& surfaces,
    std::optional<RasterPreset> rasterOverride);