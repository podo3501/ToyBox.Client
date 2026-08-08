#pragma once
#include "FontAtlasBucket.h"

class MTSDFFontAtlasBucket : public FontAtlasBucket
{
public:
    using FontAtlasBucket::FontAtlasBucket;

    virtual void EnsureGlyphs(
        const ShapedText& shapedText,
        std::vector<std::vector<GlyphUploadEntry>>& outUploadsPerPage) override;

private:
    GlyphPixelFormat GetAtlasPageFormat() const;
};