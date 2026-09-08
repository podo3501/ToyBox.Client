#pragma once

struct OverlayViewDrawList;
struct RenderUIItem;
class TextSystem;
class TransientMeshProvider;

std::optional<RenderUIItem> BuildUIItems(
    OverlayViewDrawList& draws,
    TextSystem& textSystem,
    TransientMeshProvider& meshProvider);