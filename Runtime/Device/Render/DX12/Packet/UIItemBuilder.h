#pragma once

struct ViewDrawList;
struct RenderUIItem;
class TextSystem;
class TransientMeshProvider;

std::vector<RenderUIItem> BuildUIItems(
    ViewDrawList& draws,
    TextSystem& textSystem,
    TransientMeshProvider& meshProvider);