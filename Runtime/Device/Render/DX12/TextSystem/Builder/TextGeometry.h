#pragma once
#include "TextBatch.h"

struct ShapedText;
struct RenderTextItem;
class FontAtlas;

void AppendShapedText(
    const FontAtlas& atlas,
    const ShapedText& shaped,
    const RenderTextItem& item,
    TextBatchBufferMap& buffers);