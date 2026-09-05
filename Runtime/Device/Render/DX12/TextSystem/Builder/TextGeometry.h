#pragma once

struct ShapedText;
struct RenderTextItem;
struct UIBatchBuffer;
class FontAtlas;

void AppendShapedText(
    const FontAtlas& atlas,
    const ShapedText& shaped,
    const RenderTextItem& item,
    UIBatchBuffer& buffer);