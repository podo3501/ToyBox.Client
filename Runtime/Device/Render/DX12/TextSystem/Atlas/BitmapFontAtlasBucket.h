#pragma once
#include "FontAtlasBucket.h"

class BitmapFontAtlasBucket : public FontAtlasBucket
{
public:
    using FontAtlasBucket::FontAtlasBucket;

    virtual void EnsureGlyphs(
        const ShapedText& shapedText,
        std::vector<std::vector<GlyphUploadEntry>>& outUploadsPerPage) override;

private:
    virtual AtlasPageDesc GetAtlasPageDesc() const override;
};