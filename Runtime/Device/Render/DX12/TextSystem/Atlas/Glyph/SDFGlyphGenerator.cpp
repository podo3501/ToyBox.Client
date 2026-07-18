#include "pch.h"
#include "SDFGlyphGenerator.h"
#include "Resource/Font/FontResource.h"
#include "msdfgen/msdfgen.h"
#include "msdfgen/msdfgen-ext.h"

GlyphBitmap SDFGlyphGenerator::Generate(
    FontResource* font,
    uint32_t glyphIndex,
    uint32_t size)
{
    Assert(font);

    FT_GlyphSlot slot = font->GetGlyphSlot(glyphIndex, size);
    Assert(slot);

    GlyphBitmap bitmap;

    bitmap.bearingX = static_cast<float>(slot->bitmap_left);
    bitmap.bearingY = static_cast<float>(slot->bitmap_top);
    bitmap.advanceX = static_cast<float>(slot->advance.x >> 6);

    if (slot->format != FT_GLYPH_FORMAT_OUTLINE)
        return bitmap;

    auto bucketID = GetFontBucketID(size);
    uint32_t resolution = GetSDFResolution(bucketID);
    bitmap.width = resolution;
    bitmap.height = resolution;
    bitmap.channels = 1;
    msdfgen::Bitmap<float, 1> sdf(bitmap.width, bitmap.height);

    auto shape = LoadShape(slot);
    auto bounds = shape.getBounds();
    bitmap.glyphWidth = static_cast<float>(bounds.r - bounds.l);
    bitmap.glyphHeight = static_cast<float>(bounds.t - bounds.b);
    auto projection = CreateProjection(
        shape,
        bitmap.width,
        bitmap.height);

    double range = GetSDFRange(bucketID);
    msdfgen::generateSDF(
        sdf,
        shape,
        projection,
        range);

    bitmap.pixels.resize(bitmap.width * bitmap.height);
    for (uint32_t y = 0; y < bitmap.height; ++y)
    {
        for (uint32_t x = 0; x < bitmap.width; ++x)
        {
            float v = *sdf(x, y);
            v = std::clamp(v * 0.5f + 0.5f, 0.0f, 1.0f);
            bitmap.pixels[y * bitmap.width + x] = static_cast<uint8_t>(v * 255.0f);
        }
    }

    return bitmap;
}

msdfgen::Shape SDFGlyphGenerator::LoadShape(FT_GlyphSlot slot) const
{
    Assert(slot);
    Assert(slot->format == FT_GLYPH_FORMAT_OUTLINE);

    msdfgen::Shape shape;
    FT_Error error = msdfgen::readFreetypeOutline(shape, &slot->outline);
    Assert(error == 0);

    shape.normalize();
    return shape;
}

msdfgen::Projection SDFGlyphGenerator::CreateProjection(
    const msdfgen::Shape& shape,
    uint32_t width,
    uint32_t height) const
{
    auto bounds = shape.getBounds();

    double glyphWidth = bounds.r - bounds.l;
    double glyphHeight = bounds.t - bounds.b;

    if (glyphWidth <= 0.0 || glyphHeight <= 0.0)
    {
        return msdfgen::Projection(
            msdfgen::Vector2(1.0, 1.0),
            msdfgen::Vector2(0.0, 0.0));
    }

    constexpr double Padding = 4.0;

    double availableWidth = static_cast<double>(width) - Padding * 2.0;
    double availableHeight = static_cast<double>(height) - Padding * 2.0;

    double scale = std::min(
        availableWidth / glyphWidth,
        availableHeight / glyphHeight);

    double centerX = (bounds.l + bounds.r) * 0.5;
    double centerY = (bounds.b + bounds.t) * 0.5;

    double bitmapCenterX = static_cast<double>(width) * 0.5;
    double bitmapCenterY = static_cast<double>(height) * 0.5;

    msdfgen::Vector2 translate(
        bitmapCenterX - centerX * scale,
        bitmapCenterY - centerY * scale);

    return msdfgen::Projection(
        msdfgen::Vector2(scale, scale),
        translate);
}