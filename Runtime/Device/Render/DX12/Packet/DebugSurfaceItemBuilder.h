#pragma once

struct RenderDebugSurfaceItem;
struct DrawDebugSurfaceItem;

std::vector<RenderDebugSurfaceItem> BuildDebugSurfaceItems(
    std::vector<DrawDebugSurfaceItem>& debugSurfaces);
