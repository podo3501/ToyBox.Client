#include "pch.h"
#include "MTSDFGlyphGenerator.h"
#include "Resource/Font/FontResource.h"
#include "msdfgen/msdfgen.h"
#include "msdfgen/msdfgen-ext.h"

MTSDFGlyph MTSDFGlyphGenerator::Generate(FontResource* font, uint32_t glyphIndex, uint32_t size)
{
    MTSDFGlyph result;

    FT_Face ftFace = font->GetFtFace();
    if (!ftFace || size == 0)
        return result;

    // 요청 사이즈 -> 버킷 대표 해상도(bakeSize)로 변환.
    // SDFGlyphGenerator와 동일하게, 실제 굽기는 이 bakeSize 하나로만 수행된다.
    const uint32_t bakeSize = GetMTSDFResolution(size);

    // FT_Face를 msdfgen에 그대로 위임
    msdfgen::FontHandle* msdfFont = msdfgen::adoptFreetypeFont(ftFace);
    if (!msdfFont)
        return result;

    msdfgen::Shape shape;
    double advance = 0.0;

    bool loaded = msdfgen::loadGlyph(
        shape,
        msdfFont,
        msdfgen::GlyphIndex(glyphIndex),
        msdfgen::FONT_SCALING_EM_NORMALIZED,
        &advance);

    if (!loaded)
    {
        msdfgen::destroyFont(msdfFont);
        return result;
    }

    // 공백 등 윤곽선이 없는 글리프 -> advance만 채워서 반환 (bakeSize 기준)
    if (shape.contours.empty())
    {
        result.metrics.advanceX = static_cast<float>(advance) * static_cast<float>(bakeSize);
        msdfgen::destroyFont(msdfFont);
        return result;
    }

    shape.normalize();
    // MSDF는 edge마다 색상(채널)을 배정해야 median 재구성이 가능함.
    // SDF와 달리 이 색상 배정이 결과 품질에 직접적으로 영향을 줌.
    msdfgen::edgeColoringSimple(shape, 3.0);

    msdfgen::Shape::Bounds bounds = shape.getBounds();

    // pxRange/padding도 bakeSize 기준으로 계산 (버킷 내 캐시 일관성 유지)
    constexpr double kFillRangeAt40 = 2.0;  // 순수 fill/AA용 최소 range
    constexpr double kOutlineRangeAt40 = 6.0;  // 지원할 최대 outline 두께(px, size=32 기준). 필요치에 맞게 조정
    constexpr double kBasePxRangeAt40 = kFillRangeAt40 + kOutlineRangeAt40 * 2.0;

    double pxRange = kBasePxRangeAt40 * (static_cast<double>(bakeSize) / 40.0);
    pxRange = std::clamp(pxRange, 2.0, 64.0);

    const int padding = static_cast<int>(std::ceil(pxRange * 0.5));

    const double glyphWidthPx = (bounds.r - bounds.l) * bakeSize;
    const double glyphHeightPx = (bounds.t - bounds.b) * bakeSize;

    const double rawWidth = glyphWidthPx + padding * 2;
    const double rawHeight = glyphHeightPx + padding * 2;

    int width = std::max(1, static_cast<int>(std::ceil(rawWidth)));
    int height = std::max(1, static_cast<int>(std::ceil(rawHeight)));

    // 올림 나머지를 위/아래(좌/우)에 반씩 나눠서 대칭 배치 (1px 밀림 방지)
    const double marginX = (width - glyphWidthPx) * 0.5;
    const double marginY = (height - glyphHeightPx) * 0.5;

    msdfgen::Vector2 translate(
        -bounds.l + marginX / bakeSize,
        -bounds.b + marginY / bakeSize);

    msdfgen::Projection projection(static_cast<double>(bakeSize), translate);
    msdfgen::Range range(pxRange / static_cast<double>(bakeSize));

    // RGB = MSDF(코너 보존), A = true SDF -> generateMTSDF가 한 번에 채워줌
    msdfgen::Bitmap<float, 4> mtsdf(width, height);

    msdfgen::MSDFGeneratorConfig config;
    config.errorCorrection.mode = msdfgen::ErrorCorrectionConfig::EDGE_PRIORITY;
    config.errorCorrection.distanceCheckMode = msdfgen::ErrorCorrectionConfig::CHECK_DISTANCE_AT_EDGE;

    msdfgen::generateMTSDF(
        mtsdf,
        shape,
        msdfgen::SDFTransformation(projection, range),
        config);

    msdfgen::simulate8bit(mtsdf); // 실제 8bit RGBA 텍스처로 저장될 때의 양자화를 미리 반영

    // ---- float MTSDF(4채널) -> RGBA8 버퍼로 변환 ----
    result.pixels.format = GlyphPixelFormat::RGBA8;
    result.pixels.width = static_cast<uint32_t>(width);
    result.pixels.height = static_cast<uint32_t>(height);
    result.pixels.buffer.resize(static_cast<size_t>(width) * height * 4);

    for (int y = 0; y < height; ++y)
    {
        // msdfgen은 y=0이 아래쪽 기준이라, 일반적인 이미지 좌표계(위쪽이 0)로 뒤집어서 저장
        const int srcRow = height - 1 - y;
        for (int x = 0; x < width; ++x)
        {
            const float* texel = mtsdf(x, srcRow); // texel[0..3] = R,G,B,A (0.0~1.0)
            const size_t dstIndex = (static_cast<size_t>(y) * width + x) * 4;

            for (int c = 0; c < 4; ++c)
            {
                const int byteValue = static_cast<int>(std::lround(texel[c] * 255.0f));
                result.pixels.buffer[dstIndex + c] =
                    static_cast<uint8_t>(std::clamp(byteValue, 0, 255));
            }
        }
    }

    result.metrics.width = static_cast<float>(width);
    result.metrics.height = static_cast<float>(height);
    result.metrics.pxRange = static_cast<float>(pxRange);

    result.metrics.bearingX = static_cast<float>(bounds.l * bakeSize) - static_cast<float>(marginX);
    result.metrics.bearingY = static_cast<float>(bounds.t * bakeSize) + static_cast<float>(marginY);
    result.metrics.advanceX = static_cast<float>(advance) * static_cast<float>(bakeSize);

    msdfgen::destroyFont(msdfFont);
    return result;
}