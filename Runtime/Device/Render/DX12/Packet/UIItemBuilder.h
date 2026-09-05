#pragma once

struct ViewDrawList;
struct RenderUIItem;
class TextSystem;
class TransientMeshProvider;

std::optional<RenderUIItem> BuildUIItems(
    ViewDrawList& draws,
    TextSystem& textSystem,
    TransientMeshProvider& meshProvider);