#include "pch.h"
#include "SDFGlyphGenerator.h"
#include "Resource/Font/FontResource.h"
#include "msdfgen/msdfgen.h"
#include "msdfgen/msdfgen-ext.h"

static FontBucketID GetFontBucketID(uint32_t size)
{
    if (size <= SDFBuckets::Small) return SDFBuckets::Small;
    if (size <= SDFBuckets::Medium) return SDFBuckets::Medium;
    if (size <= SDFBuckets::Large) return SDFBuckets::Large;
    if (size <= SDFBuckets::Huge) return SDFBuckets::Huge;
    Assert(false);

    return SDFBuckets::Medium;
}

static uint32_t GetSDFResolution(FontBucketID bucket)
{
    switch (bucket)
    {
    case SDFBuckets::Small:  return 40;
    case SDFBuckets::Medium: return 60;
    case SDFBuckets::Large:  return 80;
    case SDFBuckets::Huge:  return 100;
    }
    Assert(false);

    return SDFBuckets::Medium;
}

static double GetSDFRange(FontBucketID bucket)
{
    switch (bucket)
    {
    case SDFBuckets::Small: return 3.0;
    case SDFBuckets::Medium: return 4.0;
    case SDFBuckets::Large: return 6.0;
    case SDFBuckets::Huge: return 8.0;
    }
    Assert(false);

    return 4.0;
}

SDFGlyph SDFGlyphGenerator::Generate(
    FontResource* font,
    uint32_t glyphIndex,
    uint32_t size)
{
    Assert(font);

    FT_GlyphSlot slot = font->GetGlyphOutlineSlot(glyphIndex, size);
    Assert(slot);

    SDFGlyph sdfGlyph;
    auto& pixels = sdfGlyph.pixels;
    auto& metrics = sdfGlyph.metrics;
    auto bucketID = GetFontBucketID(size);
    uint32_t resolution = GetSDFResolution(bucketID);

    pixels.format = GlyphPixelFormat::R8;
    pixels.width = resolution;
    pixels.height = resolution;

    auto shape = LoadShape(slot);
    auto bounds = shape.getBounds();
    if (bounds.l >= bounds.r || bounds.b >= bounds.t)
        return sdfGlyph;  //아웃라인 경계가 없는 빈 글자 처리.

    metrics.width = static_cast<float>(bounds.r - bounds.l);
    metrics.height = static_cast<float>(bounds.t - bounds.b);
    metrics.bearingX = static_cast<float>(slot->bitmap_left);
    metrics.bearingY = static_cast<float>(slot->bitmap_top);
    metrics.advanceX = static_cast<float>(slot->advance.x >> 6);

    if (slot->format != FT_GLYPH_FORMAT_OUTLINE)
        return sdfGlyph;

    auto projection = CreateProjection(
        shape,
        pixels.width,
        pixels.height);

    msdfgen::Bitmap<float, 1> sdf(pixels.width, pixels.height);
    double range = GetSDFRange(bucketID);
    range = 5.0;

    msdfgen::SDFTransformation transformation(
        projection,
        msdfgen::Range(range));

    msdfgen::generateSDF(
        sdf,
        shape,
        transformation);

    float minValue = FLT_MAX;
    float maxValue = -FLT_MAX;

    size_t pixelCount = pixels.width * pixels.height * GetBytesPerPixel(pixels.format);
    pixels.buffer.resize(pixelCount);

    float inverseRange = 1.0f / static_cast<float>(range);
    for (uint32_t y = 0; y < pixels.height; ++y)
    {
        uint32_t srcY = pixels.height - 1 - y; // msdfgen 소스 버퍼에서 읽어올 거꾸로 된 Y축 위치
        for (uint32_t x = 0; x < pixels.width; ++x)
        {
            float raw = *sdf(x, srcY);
            float v = std::clamp(raw * inverseRange + 0.5f, 0.0f, 1.0f);
            pixels.buffer[y * pixels.width + x] = static_cast<uint8_t>(v * 255.0f);

            minValue = std::min(minValue, raw);
            maxValue = std::max(maxValue, raw);
        }
    }

    return sdfGlyph;
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

    constexpr double BiasX = 0.0;
    constexpr double BiasY = 0.0;

    msdfgen::Vector2 translate(
        static_cast<double>(width) * 0.5 - centerX * scale - BiasX,
        static_cast<double>(height) * 0.5 - centerY * scale - BiasY);

    return msdfgen::Projection(
        msdfgen::Vector2(scale, scale),
        translate);
}