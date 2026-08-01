#include "pch.h"
#include "MTSDFGlyphGenerator.h"
#include "Resource/Font/FontResource.h"
#include "msdfgen/msdfgen.h"
#include "msdfgen/msdfgen-ext.h"

struct MsdfFontDeleter
{
    void operator()(msdfgen::FontHandle* fontHandle) const
    {
        if (fontHandle)
            msdfgen::destroyFont(fontHandle);
    }
};
using MsdfFontPtr = std::unique_ptr<msdfgen::FontHandle, MsdfFontDeleter>;

// 레이아웃 계산 결과를 한데 묶는 구조체
struct MTSDFLayout
{
    double pxRange = 0.0;
    int width = 0;
    int height = 0;
    double marginX = 0.0; // bearingX 계산에 필요
    double marginY = 0.0; // bearingY 계산에 필요
    msdfgen::Vector2 translate;
};

// FreeType face에서 glyph의 Shape과 advance를 로드. 실패 시 false 반환.
static bool LoadShape(
    msdfgen::FontHandle* msdfFont,
    uint32_t glyphIndex,
    msdfgen::Shape& outShape,
    double& outAdvance)
{
    return msdfgen::loadGlyph(
        outShape,
        msdfFont,
        msdfgen::GlyphIndex(glyphIndex),
        msdfgen::FONT_SCALING_EM_NORMALIZED,
        &outAdvance);
}

// bounds로부터 pxRange/padding/비트맵 크기/translate/margin을 계산.
// bakeSize 기준으로 계산해 버킷 내 캐시 일관성을 유지.
static MTSDFLayout ComputeMTSDFLayout(const msdfgen::Shape::Bounds& bounds, uint32_t bakeSize)
{
    constexpr double kFillRangeAt40 = 2.0;   // 순수 fill/AA용 최소 range
    constexpr double kOutlineRangeAt40 = 6.0; // 지원할 최대 outline 두께(px, size=40 기준)
    constexpr double kBasePxRangeAt40 = kFillRangeAt40 + kOutlineRangeAt40 * 2.0;
    constexpr double kReferenceBakeSize = 40.0; // pxRange 계산 기준 해상도
    constexpr double kMinPxRange = 1.0;
    constexpr double kMaxPxRange = 64.0;

    MTSDFLayout layout;
    layout.pxRange = kBasePxRangeAt40 * (static_cast<double>(bakeSize) / kReferenceBakeSize);
    layout.pxRange = std::clamp(layout.pxRange, kMinPxRange, kMaxPxRange);

    const int padding = static_cast<int>(std::ceil(layout.pxRange * 0.5));

    const double glyphWidthPx = (bounds.r - bounds.l) * bakeSize;
    const double glyphHeightPx = (bounds.t - bounds.b) * bakeSize;

    const double rawWidth = glyphWidthPx + padding * 2;
    const double rawHeight = glyphHeightPx + padding * 2;

    layout.width = std::max(1, static_cast<int>(std::ceil(rawWidth)));
    layout.height = std::max(1, static_cast<int>(std::ceil(rawHeight)));

    // 올림 나머지를 위/아래(좌/우)에 반씩 나눠서 대칭 배치 (1px 밀림 방지)
    layout.marginX = (layout.width - glyphWidthPx) * 0.5;
    layout.marginY = (layout.height - glyphHeightPx) * 0.5;

    layout.translate = msdfgen::Vector2(
        -bounds.l + layout.marginX / bakeSize,
        -bounds.b + layout.marginY / bakeSize);

    return layout;
}

// Shape + layout으로 MTSDF 비트맵을 실제 렌더링.
static msdfgen::Bitmap<float, 4> RenderMTSDFBitmap(
    msdfgen::Shape& shape,
    const MTSDFLayout& layout,
    uint32_t bakeSize)
{
    msdfgen::Projection projection(static_cast<double>(bakeSize), layout.translate);
    msdfgen::Range range(layout.pxRange / static_cast<double>(bakeSize));

    // RGB = MSDF(코너 보존), A = true SDF -> generateMTSDF가 한 번에 채워줌
    msdfgen::Bitmap<float, 4> mtsdf(layout.width, layout.height);

    msdfgen::MSDFGeneratorConfig config;
    config.errorCorrection.mode = msdfgen::ErrorCorrectionConfig::EDGE_PRIORITY;
    config.errorCorrection.distanceCheckMode = msdfgen::ErrorCorrectionConfig::CHECK_DISTANCE_AT_EDGE;

    msdfgen::generateMTSDF(
        mtsdf,
        shape,
        msdfgen::SDFTransformation(projection, range),
        config);

    msdfgen::simulate8bit(mtsdf); // 실제 8bit RGBA 텍스처로 저장될 때의 양자화를 미리 반영
    return mtsdf;
}

// float 4채널 MTSDF 비트맵 -> RGBA8 버퍼로 변환해 GlyphPixels에 채움.
static void ConvertMTSDFToRGBA8(const msdfgen::Bitmap<float, 4>& mtsdf, int width, int height, GlyphPixels& outPixels)
{
    outPixels.format = GlyphPixelFormat::RGBA8;
    outPixels.width = static_cast<uint32_t>(width);
    outPixels.height = static_cast<uint32_t>(height);

    const size_t bytesPerPixel = GetBytesPerPixel(outPixels.format);
    Assert(bytesPerPixel == 4); // 이 함수는 RGBA8(4바이트/픽셀) 포맷만 지원
    outPixels.buffer.resize(static_cast<size_t>(width) * height * bytesPerPixel);

    for (int y = 0; y < height; ++y)
    {
        // msdfgen은 y=0이 아래쪽 기준이라, 일반적인 이미지 좌표계(위쪽이 0)로 뒤집어서 저장
        const int srcRow = height - 1 - y;
        const float* rowStart = mtsdf(0, srcRow);
        uint8_t* dstRow = outPixels.buffer.data() + static_cast<size_t>(y) * width * bytesPerPixel;

        for (int x = 0; x < width; ++x)
        {
            const float* texel = rowStart + x * 4; // texel[0..3] = R,G,B,A (0.0~1.0)
            uint8_t* dstTexel = dstRow + x * bytesPerPixel;

            for (int c = 0; c < 4; ++c)
            {
                const int byteValue = static_cast<int>(std::lround(texel[c] * 255.0f));
                dstTexel[c] = static_cast<uint8_t>(std::clamp(byteValue, 0, 255));
            }
        }
    }
}

MTSDFGlyph GenerateMTSDFGlyph(FontResource* font, uint32_t glyphIndex, uint32_t size)
{
    MTSDFGlyph result;

    if (!font)
    {
        Assert(false); // Invalid font
        return result;
    }

    FT_Face ftFace = font->GetFtFace();
    if (!ftFace || size == 0)
        return result;

    const uint32_t bakeSize = GetMTSDFResolution(size);
    MsdfFontPtr msdfFont(msdfgen::adoptFreetypeFont(ftFace));
    if (!msdfFont)
        return result;

    msdfgen::Shape shape;
    double advance = 0.0;
    if (!LoadShape(msdfFont.get(), glyphIndex, shape, advance))
        return result;

    // advance는 이후 모든 경로(공백 glyph 포함)에서 공통으로 필요하므로 한 번만 계산
    result.metrics.advanceX = static_cast<float>(advance) * static_cast<float>(bakeSize);

    // 공백 등 윤곽선이 없는 글리프 -> advance만 채워서 반환
    if (shape.contours.empty())
        return result;

    shape.normalize();
    // MSDF는 edge마다 색상(채널)을 배정해야 median 재구성이 가능함.
    // SDF와 달리 이 색상 배정이 결과 품질에 직접적으로 영향을 줌.
    msdfgen::edgeColoringSimple(shape, 3.0);
    msdfgen::Shape::Bounds bounds = shape.getBounds();

    const MTSDFLayout layout = ComputeMTSDFLayout(bounds, bakeSize);
    msdfgen::Bitmap<float, 4> mtsdf = RenderMTSDFBitmap(shape, layout, bakeSize);
    ConvertMTSDFToRGBA8(mtsdf, layout.width, layout.height, result.pixels);

    result.metrics.width = static_cast<float>(layout.width);
    result.metrics.height = static_cast<float>(layout.height);
    result.metrics.pxRange = static_cast<float>(layout.pxRange);
    result.metrics.bearingX = static_cast<float>(bounds.l * bakeSize) - static_cast<float>(layout.marginX);
    result.metrics.bearingY = static_cast<float>(bounds.t * bakeSize) + static_cast<float>(layout.marginY);

    return result;
}