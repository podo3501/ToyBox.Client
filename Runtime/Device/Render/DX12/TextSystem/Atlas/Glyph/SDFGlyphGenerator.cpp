#include "pch.h"
#include "SDFGlyphGenerator.h"
#include "Resource/Font/FontResource.h"
#include "msdfgen/msdfgen.h"
#include "msdfgen/msdfgen-ext.h"


SDFGlyph SDFGlyphGenerator::Generate(FontResource* font, uint32_t glyphIndex, uint32_t size)
{
    SDFGlyph result;

    FT_Face ftFace = font->GetFtFace();
    if (!ftFace || size == 0)
        return result;

    // 요청 사이즈 -> 버킷 -> 버킷 대표 해상도(bakeSize)로 변환.
    // 실제 SDF는 이 bakeSize 하나로만 구워지고, 같은 버킷 안의
    // 모든 요청 사이즈는 이 텍스처를 공유해서 렌더링 시점에만 스케일된다.
    const uint32_t bakeSize = GetSDFResolution(size);

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
    msdfgen::edgeColoringSimple(shape, 3.0);

    msdfgen::Shape::Bounds bounds = shape.getBounds();

    // pxRange/padding도 요청 size가 아니라 bakeSize 기준으로 계산해야
    // 같은 버킷 안에서 항상 동일한 텍스처가 나옴 (캐싱 전제 조건)
    constexpr double kBasePxRangeAt32 = 4.0;
    double pxRange = kBasePxRangeAt32 * (static_cast<double>(bakeSize) / 32.0);
    pxRange = std::clamp(pxRange, 2.0, 32.0);

    const int padding = static_cast<int>(std::ceil(pxRange * 0.5));

    const double glyphWidthPx = (bounds.r - bounds.l) * bakeSize;
    const double glyphHeightPx = (bounds.t - bounds.b) * bakeSize;

    const double rawWidth = glyphWidthPx + padding * 2;
    const double rawHeight = glyphHeightPx + padding * 2;

    int width = std::max(1, static_cast<int>(std::ceil(rawWidth)));
    int height = std::max(1, static_cast<int>(std::ceil(rawHeight)));

    const double marginX = (width - glyphWidthPx) * 0.5;
    const double marginY = (height - glyphHeightPx) * 0.5;

    msdfgen::Vector2 translate(
        -bounds.l + marginX / bakeSize,
        -bounds.b + marginY / bakeSize);

    msdfgen::Projection projection(static_cast<double>(bakeSize), translate);
    msdfgen::Range range(pxRange / static_cast<double>(bakeSize));

    msdfgen::Bitmap<float, 1> sdf(width, height);
    msdfgen::generateSDF(sdf, shape, msdfgen::SDFTransformation(projection, range));
    msdfgen::simulate8bit(sdf);

    result.pixels.format = GlyphPixelFormat::R8;
    result.pixels.width = static_cast<uint32_t>(width);
    result.pixels.height = static_cast<uint32_t>(height);
    result.pixels.buffer.resize(static_cast<size_t>(width) * height);

    for (int y = 0; y < height; ++y)
    {
        const int srcRow = height - 1 - y;
        for (int x = 0; x < width; ++x)
        {
            const float value = sdf(x, srcRow)[0];
            const int byteValue = static_cast<int>(std::lround(value * 255.0f));
            result.pixels.buffer[static_cast<size_t>(y) * width + x] =
                static_cast<uint8_t>(std::clamp(byteValue, 0, 255));
        }
    }

    // 여기 저장되는 모든 메트릭은 bakeSize 기준값이다.
    // 실제 요청 size로 렌더링할 때는 (size / bakeSize) 배율을 곱해서 써야 한다.
    result.metrics.width = static_cast<float>(width);
    result.metrics.height = static_cast<float>(height);
    result.metrics.bearingX = static_cast<float>(bounds.l * bakeSize) - static_cast<float>(marginX);
    result.metrics.bearingY = static_cast<float>(bounds.t * bakeSize) + static_cast<float>(marginY);
    result.metrics.advanceX = static_cast<float>(advance) * static_cast<float>(bakeSize);

    msdfgen::destroyFont(msdfFont);
    return result;
}

//
//SDFGlyph SDFGlyphGenerator::Generate(FontResource* font, uint32_t glyphIndex, uint32_t size)
//{
//    SDFGlyph result;
//
//    FT_Face ftFace = font->GetFtFace();
//    if (!ftFace || size == 0)
//        return result;
//
//    // FT_Face를 msdfgen에 그대로 위임 (소유권은 유지, destroyFont 해도 face는 안 죽음)
//    msdfgen::FontHandle* msdfFont = msdfgen::adoptFreetypeFont(ftFace);
//    if (!msdfFont)
//        return result;
//
//    msdfgen::Shape shape;
//    double advance = 0.0;
//
//    // 최신 API: glyph index 기반 로드는 GlyphIndex 오버로드를 써야 함
//    bool loaded = msdfgen::loadGlyph(
//        shape,
//        msdfFont,
//        msdfgen::GlyphIndex(glyphIndex),
//        msdfgen::FONT_SCALING_EM_NORMALIZED, // em 정규화 좌표계(0~1)로 로드
//        &advance);
//
//    if (!loaded)
//    {
//        msdfgen::destroyFont(msdfFont);
//        return result;
//    }
//
//    // 공백 등 윤곽선이 없는 글리프 -> advance만 채워서 반환
//    if (shape.contours.empty())
//    {
//        result.metrics.advanceX = static_cast<float>(advance) * static_cast<float>(size);
//        msdfgen::destroyFont(msdfFont);
//        return result;
//    }
//
//    shape.normalize();
//    msdfgen::edgeColoringSimple(shape, 3.0);
//
//    msdfgen::Shape::Bounds bounds = shape.getBounds();
//
//    // ---- size에 따라 pxRange / padding을 스케일링 ----
//    // 기준: size 32px일 때 pxRange 4px. size가 커지면 pxRange도 비례해서 커지되
//    // 너무 작거나(에일리어싱) 너무 크지(atlas 낭비) 않도록 clamp.
//    constexpr double kBasePxRangeAt32 = 4.0;
//    double pxRange = kBasePxRangeAt32 * (static_cast<double>(size) / 32.0);
//    pxRange = std::clamp(pxRange, 2.0, 32.0);
//
//    // 아웃라인을 쓸 거면 기준값을 올려줌 (예: 32px 기준 4px -> 8px)
//    //constexpr double kBasePxRangeAt32 = 18.0; // 필요한 최대 아웃라인 두께에 맞춰 조정
//    //double pxRange = kBasePxRangeAt32 * (static_cast<double>(size) / 32.0);
//    //pxRange = std::clamp(pxRange, 12.0, 48.0); // clamp 상한도 같이 올려줌
//
//    const int padding = static_cast<int>(std::ceil(pxRange * 0.5));
//
//
//    const double glyphWidthPx = (bounds.r - bounds.l) * size;
//    const double glyphHeightPx = (bounds.t - bounds.b) * size;
//
//    const double rawWidth = glyphWidthPx + padding * 2;
//    const double rawHeight = glyphHeightPx + padding * 2;
//
//    int width = std::max(1, static_cast<int>(std::ceil(rawWidth)));
//    int height = std::max(1, static_cast<int>(std::ceil(rawHeight)));
//
//    // 올림하면서 생긴 나머지를 위/아래(좌/우)에 반씩 나눠서 대칭으로 배치
//    const double marginX = (width - glyphWidthPx) * 0.5;
//    const double marginY = (height - glyphHeightPx) * 0.5;
//
//    msdfgen::Vector2 translate(
//        -bounds.l + marginX / size,
//        -bounds.b + marginY / size);
//
//    msdfgen::Projection projection(static_cast<double>(size), translate);
//    msdfgen::Range range(pxRange / static_cast<double>(size));
//
//
//
//    //// em 단위 bounds -> size(px) 스케일로 변환한 실제 글리프 픽셀 크기
//    //const double glyphWidthPx = (bounds.r - bounds.l) * size;
//    //const double glyphHeightPx = (bounds.t - bounds.b) * size;
//
//    //int width = static_cast<int>(std::ceil(glyphWidthPx)) + padding * 2;
//    //int height = static_cast<int>(std::ceil(glyphHeightPx)) + padding * 2;
//    //width = std::max(width, 1);
//    //height = std::max(height, 1);
//
//    //// bounds.l, bounds.b를 원점으로 맞추고, padding만큼(em 단위로 환산해서) 밀어줌
//    //msdfgen::Vector2 translate(
//    //    -bounds.l + padding / static_cast<double>(size),
//    //    -bounds.b + padding / static_cast<double>(size));
//
//    //msdfgen::Projection projection(static_cast<double>(size), translate);
//    //msdfgen::Range range(pxRange / static_cast<double>(size)); // px range -> em range로 변환
//
//    msdfgen::Bitmap<float, 1> sdf(width, height);
//    msdfgen::generateSDF(sdf, shape, msdfgen::SDFTransformation(projection, range));
//    msdfgen::simulate8bit(sdf); // 실제 8bit 텍스처로 저장될 때의 양자화를 미리 반영
//
//    // ---- float SDF -> R8 버퍼로 변환 ----
//    result.pixels.format = GlyphPixelFormat::R8;
//    result.pixels.width = static_cast<uint32_t>(width);
//    result.pixels.height = static_cast<uint32_t>(height);
//    result.pixels.buffer.resize(static_cast<size_t>(width) * height);
//
//    for (int y = 0; y < height; ++y)
//    {
//        // msdfgen은 y=0이 아래쪽 기준이라, 일반적인 이미지 좌표계(위쪽이 0)로 뒤집어서 저장
//        const int srcRow = height - 1 - y;
//        for (int x = 0; x < width; ++x)
//        {
//            const float value = sdf(x, srcRow)[0]; // 0.0~1.0
//            const int byteValue = static_cast<int>(std::lround(value * 255.0f));
//            result.pixels.buffer[static_cast<size_t>(y) * width + x] =
//                static_cast<uint8_t>(std::clamp(byteValue, 0, 255));
//        }
//    }
//
//    result.metrics.width = static_cast<float>(width);
//    result.metrics.height = static_cast<float>(height);
//    //result.metrics.bearingX = static_cast<float>(bounds.l * size) - static_cast<float>(padding);
//    //result.metrics.bearingY = static_cast<float>(bounds.t * size) + static_cast<float>(padding);
//
//    result.metrics.bearingX = static_cast<float>(bounds.l * size) - static_cast<float>(marginX);
//    result.metrics.bearingY = static_cast<float>(bounds.t * size) + static_cast<float>(marginY);
//
//    result.metrics.advanceX = static_cast<float>(advance) * static_cast<float>(size);
//
//    msdfgen::destroyFont(msdfFont);
//    return result;
//}
//








//namespace
//{
//    constexpr int SDFPadding = 20;         // 여백 크기
//    constexpr double SDFRange = 20.0;      // 거리 계산 레인지 (픽셀 범위)
//}
//
//SDFGlyph SDFGlyphGenerator::Generate(
//    FontResource* font,
//    uint32_t glyphIndex,
//    uint32_t size)
//{
//    Assert(font);
//    FT_Face ftFace = font->GetFtFace(); // FontResource에 m_ftFace를 가져오는 getter가 있다고 가정
//    Assert(ftFace);
//
//    SDFGlyph sdfGlyph;
//    auto& pixels = sdfGlyph.pixels;
//    auto& metrics = sdfGlyph.metrics;
//
//    msdfgen::Shape shape;
//    msdfgen::FontHandle* fontHandle = msdfgen::adoptFreetypeFont(ftFace);
//    if (fontHandle)
//    {
//        // 2. GlyphIndex 객체로 감싸서 로드 (버전에 따라 unicode 혹은 GlyphIndex 오버로드 사용)
//        msdfgen::GlyphIndex glyph(glyphIndex);
//
//        double advance = 0.0;
//        // FONT_SCALING_EM_NORMALIZED 또는 FONT_SCALING_NONE 등을 지정할 수 있습니다.
//        if (!msdfgen::loadGlyph(shape, fontHandle, glyph, msdfgen::FONT_SCALING_NONE, &advance))
//            return sdfGlyph;
//
//        // 3. 핸들 파기 (중요: 파기하더라도 인자로 넘긴 원본 ftFace 자체는 파기되지 않음)
//        msdfgen::destroyFont(fontHandle);
//    }
//
//    // 2. 윤곽선 정규화 (방향 보정 및 겹침/구멍 뚫림 방지)
//    shape.normalize();
//
//    // 3. 바운딩 박스(Bounds) 계산
//    auto bounds = shape.getBounds();
//
//    // 만약 글자 크기가 0이거나 유효하지 않다면 빈 구조체 반환
//    if (!shape.validate() || bounds.l >= bounds.r || bounds.b >= bounds.t)
//    {
//        return sdfGlyph;
//    }
//
//    // 2. 폰트의 EM 크기(디자인 단위 기준 1em이 몇 유닛인지)를 가져옵니다.
//    //msdfgen::FontMetrics metricsInfo;
//    //msdfgen::getFontMetrics(metricsInfo, fontHandle); // 또는 ftFace로부터 획득 가능
//    // 일반적으로 OpenType 폰트는 1000 또는 2048 유닛을 1em으로 씁니다.
//    // FreeType ftFace->units_per_EM 값을 직접 쓰는 것이 가장 안전합니다.
//    double unitsPerEM = static_cast<double>(ftFace->units_per_EM);
//    if (unitsPerEM <= 0.0) unitsPerEM = 2048.0; // 안전 장치
//
//    // 3. 원하는 픽셀 크기(size)에 맞추기 위한 스케일 팩터 계산
//    double pxScale = static_cast<double>(size) / unitsPerEM;
//
//    // 4. 바운딩 박스 및 패딩을 고려한 최종 픽셀 비트맵 크기 계산
//    int w = static_cast<int>(std::ceil((bounds.r - bounds.l) * pxScale)) + SDFPadding * 2;
//    int h = static_cast<int>(std::ceil((bounds.t - bounds.b) * pxScale)) + SDFPadding * 2;
//
//    //// 4. 패딩을 포함한 최종 비트맵 크기 산정
//    //// msdfgen은 기본적으로 스케일과 트랜슬레이션을 고려해 비트맵을 생성합니다.
//    //int w = static_cast<int>(std::ceil(bounds.r - bounds.l)) + SDFPadding * 2;
//    //int h = static_cast<int>(std::ceil(bounds.t - bounds.b)) + SDFPadding * 2;
//
//    // 최소 크기 보장
//    w = std::max(w, 1);
//    h = std::max(h, 1);
//
//    pixels.format = GlyphPixelFormat::R8; // 우선 단일 채널 SDF로 테스트 (추후 RGBA8로 변경 시 MSDF 가능)
//    pixels.width = static_cast<uint32_t>(w);
//    pixels.height = static_cast<uint32_t>(h);
//
//    metrics.width = static_cast<float>((bounds.r - bounds.l) * pxScale);
//    metrics.height = static_cast<float>((bounds.t - bounds.b) * pxScale);
//    metrics.bearingX = static_cast<float>(bounds.l * pxScale) - static_cast<float>(SDFPadding);
//    metrics.bearingY = static_cast<float>(bounds.t * pxScale) + static_cast<float>(SDFPadding); // Y축 배치 기준에 맞게 조정
//
//    // Advance X는 FreeType 슬롯에서 직접 가져오는 것이 안전합니다.
//    FT_Load_Glyph(ftFace, glyphIndex, FT_LOAD_DEFAULT);
//    metrics.advanceX = static_cast<float>(ftFace->glyph->advance.x >> 6);
//
//    size_t pixelCount = pixels.width * pixels.height * GetBytesPerPixel(pixels.format);
//    pixels.buffer.resize(pixelCount);
//
//    // 5. msdfgen 비트맵 컨테이너 생성 (단일 채널 Bitmap 타입: msdfgen::Bitmap<float, 1>)
//    msdfgen::Bitmap<float, 1> bitmap(w, h);
//
//    // 스케일과 트랜슬레이션 설정 (패딩과 레인지 고려)
//    // msdfgen의 좌표계 스케일 지정 방식 적용
//    //double scale = static_cast<double>(size) / 64.0; // 또는 폰트 스케일에 맞춘 적절한 값
//    // 편의상 1:1 픽셀 매칭 기준 스케일 배율 설정
//    // 실무에서는 폰트 디자인 유닛에 따른 변환 계수가 필요할 수 있습니다.
//
//    //msdfgen::Vector2 translate(-bounds.l + SDFPadding, -bounds.b + SDFPadding);
//    double generatorScale = pxScale;
//    msdfgen::Vector2 translate(
//        -bounds.l + (static_cast<double>(SDFPadding) / generatorScale), 
//        -bounds.b + (static_cast<double>(SDFPadding) / generatorScale));
//
//    // 6. 단일 채널 SDF 생성 함수 호출
//    // (만약 완전한 MSDF를 원하신다면 Bitmap<float, 3>을 쓰고 msdfgen::generateMSDF를 호출하면 됩니다.)
//    //msdfgen::generateSDF(bitmap, shape, SDFRange, generatorScale, translate);
//
//    msdfgen::Projection projection(
//        msdfgen::Vector2(generatorScale, generatorScale),
//        translate);
//
//    msdfgen::SDFTransformation transform(
//        projection,
//        msdfgen::Range(SDFRange / generatorScale));
//
//    msdfgen::generateSDF(bitmap, shape, transform);
//
//    // 7. msdfgen의 float 버퍼(0.0 ~ 1.0)를 우리 엔진의 uint8_t 버퍼로 변환 복사
//    for (int y = 0; y < h; ++y)
//    {
//        for (int x = 0; x < w; ++x)
//        {
//            // msdfgen은 좌하단(Bottom-Left) 기준일 수 있으므로, 
//            // 상하 반전이 일어난다면 y 인덱스를 (h - 1 - y)로 뒤집어야 합니다.
//            int targetY = h - 1 - y; // DirectX 상하 반전 대응
//
//            float val = *bitmap(x, y); // 0.0 ~ 1.0 실수형 거리값
//            val = std::clamp(val, 0.0f, 1.0f);
//
//            pixels.buffer[targetY * w + x] = static_cast<uint8_t>(val * 255.0f);
//        }
//    }
//
//    return sdfGlyph;
//}














//static FontBucketID GetFontBucketID(uint32_t size)
//{
//    if (size <= SDFBuckets::Small) return SDFBuckets::Small;
//    if (size <= SDFBuckets::Medium) return SDFBuckets::Medium;
//    if (size <= SDFBuckets::Large) return SDFBuckets::Large;
//    if (size <= SDFBuckets::Huge) return SDFBuckets::Huge;
//    Assert(false);
//
//    return SDFBuckets::Medium;
//}
//
//static uint32_t GetSDFResolution(FontBucketID bucket)
//{
//    switch (bucket)
//    {
//    case SDFBuckets::Small:  return 40;
//    case SDFBuckets::Medium: return 60;
//    case SDFBuckets::Large:  return 80;
//    case SDFBuckets::Huge:  return 100;
//    }
//    Assert(false);
//
//    return SDFBuckets::Medium;
//}
//
//static double GetSDFRange(FontBucketID bucket)
//{
//    switch (bucket)
//    {
//    case SDFBuckets::Small: return 3.0;
//    case SDFBuckets::Medium: return 4.0;
//    case SDFBuckets::Large: return 6.0;
//    case SDFBuckets::Huge: return 8.0;
//    }
//    Assert(false);
//
//    return 4.0;
//}
//
//SDFGlyph SDFGlyphGenerator::Generate(
//    FontResource* font,
//    uint32_t glyphIndex,
//    uint32_t size)
//{
//    Assert(font);
//
//    FT_GlyphSlot slot = font->GetGlyphOutlineSlot(glyphIndex, size);
//    Assert(slot);
//
//    SDFGlyph sdfGlyph;
//    auto& pixels = sdfGlyph.pixels;
//    auto& metrics = sdfGlyph.metrics;
//    auto bucketID = GetFontBucketID(size);
//    uint32_t resolution = GetSDFResolution(bucketID);
//
//    pixels.format = GlyphPixelFormat::R8;
//    pixels.width = resolution;
//    pixels.height = resolution;
//
//    auto shape = LoadShape(slot);
//    auto bounds = shape.getBounds();
//    if (bounds.l >= bounds.r || bounds.b >= bounds.t)
//        return sdfGlyph;  //아웃라인 경계가 없는 빈 글자 처리.
//
//    metrics.width = static_cast<float>(bounds.r - bounds.l);
//    metrics.height = static_cast<float>(bounds.t - bounds.b);
//    metrics.bearingX = static_cast<float>(slot->bitmap_left);
//    metrics.bearingY = static_cast<float>(slot->bitmap_top);
//    metrics.advanceX = static_cast<float>(slot->advance.x >> 6);
//
//    if (slot->format != FT_GLYPH_FORMAT_OUTLINE)
//        return sdfGlyph;
//
//    auto projection = CreateProjection(
//        shape,
//        pixels.width,
//        pixels.height);
//
//    msdfgen::Bitmap<float, 1> sdf(pixels.width, pixels.height);
//    double range = GetSDFRange(bucketID);
//    range = 5.0;
//
//    msdfgen::SDFTransformation transformation(
//        projection,
//        msdfgen::Range(range));
//
//    msdfgen::generateSDF(
//        sdf,
//        shape,
//        transformation);
//
//    float minValue = FLT_MAX;
//    float maxValue = -FLT_MAX;
//
//    size_t pixelCount = pixels.width * pixels.height * GetBytesPerPixel(pixels.format);
//    pixels.buffer.resize(pixelCount);
//
//    float inverseRange = 1.0f / static_cast<float>(range);
//    for (uint32_t y = 0; y < pixels.height; ++y)
//    {
//        uint32_t srcY = pixels.height - 1 - y; // msdfgen 소스 버퍼에서 읽어올 거꾸로 된 Y축 위치
//        for (uint32_t x = 0; x < pixels.width; ++x)
//        {
//            float raw = *sdf(x, srcY);
//            float v = std::clamp(raw * inverseRange + 0.5f, 0.0f, 1.0f);
//            pixels.buffer[y * pixels.width + x] = static_cast<uint8_t>(v * 255.0f);
//
//            minValue = std::min(minValue, raw);
//            maxValue = std::max(maxValue, raw);
//        }
//    }
//
//    return sdfGlyph;
//}
//
//msdfgen::Shape SDFGlyphGenerator::LoadShape(FT_GlyphSlot slot) const
//{
//    Assert(slot);
//    Assert(slot->format == FT_GLYPH_FORMAT_OUTLINE);
//
//    msdfgen::Shape shape;
//    FT_Error error = msdfgen::readFreetypeOutline(shape, &slot->outline);
//    Assert(error == 0);
//
//    shape.normalize();
//    return shape;
//}
//
//msdfgen::Projection SDFGlyphGenerator::CreateProjection(
//    const msdfgen::Shape& shape,
//    uint32_t width,
//    uint32_t height) const
//{
//    auto bounds = shape.getBounds();
//
//    double glyphWidth = bounds.r - bounds.l;
//    double glyphHeight = bounds.t - bounds.b;
//
//    if (glyphWidth <= 0.0 || glyphHeight <= 0.0)
//    {
//        return msdfgen::Projection(
//            msdfgen::Vector2(1.0, 1.0),
//            msdfgen::Vector2(0.0, 0.0));
//    }
//
//    constexpr double Padding = 4.0;
//
//    double availableWidth = static_cast<double>(width) - Padding * 2.0;
//    double availableHeight = static_cast<double>(height) - Padding * 2.0;
//
//    double scale = std::min(
//        availableWidth / glyphWidth,
//        availableHeight / glyphHeight);
//
//    double centerX = (bounds.l + bounds.r) * 0.5;
//    double centerY = (bounds.b + bounds.t) * 0.5;
//
//    constexpr double BiasX = 0.0;
//    constexpr double BiasY = 0.0;
//
//    msdfgen::Vector2 translate(
//        static_cast<double>(width) * 0.5 - centerX * scale - BiasX,
//        static_cast<double>(height) * 0.5 - centerY * scale - BiasY);
//
//    return msdfgen::Projection(
//        msdfgen::Vector2(scale, scale),
//        translate);
//}