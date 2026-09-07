#pragma once

struct OverlayDrawList;
struct RenderUIItem;
class TextSystem;
class TransientMeshProvider;

std::optional<RenderUIItem> BuildUIItems(
    OverlayDrawList& draws,
    TextSystem& textSystem,
    TransientMeshProvider& meshProvider);