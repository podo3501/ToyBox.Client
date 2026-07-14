#include "pch.h"
#include "FontResource.h"
#include "Core/FreeTypeLibrary.h"

FontResource::~FontResource()
{
    for (auto& [size, hbFont] : m_hbFonts)
        if (hbFont) hb_font_destroy(hbFont);
    m_hbFonts.clear();

    if (m_ftFace)
    {
        FT_Done_Face(m_ftFace);
        m_ftFace = nullptr;
    }
}

FontResource::FontResource() = default;

bool FontResource::Initialize(FreeTypeLibrary& ftLibrary, std::shared_ptr<FontAsset> asset)
{
    FT_Error error = FT_New_Memory_Face(
        ftLibrary.Get(),
        reinterpret_cast<const FT_Byte*>(asset->fontSource.data()),
        static_cast<FT_Long>(asset->fontSource.size()),
        0, // 단일 페이스 인덱스
        &m_ftFace);
	if (error)
		return false;

	m_asset = std::move(asset);
	MarkReady();

	return true;
}

FT_GlyphSlot FontResource::GetGlyphSlot(uint32_t glyphIndex, uint32_t size) const
{
    FT_Set_Pixel_Sizes(m_ftFace, 0, size);

    if (FT_Load_Glyph(m_ftFace, glyphIndex, FT_LOAD_DEFAULT))
        return nullptr;

    if (FT_Render_Glyph(m_ftFace->glyph, FT_RENDER_MODE_NORMAL))
        return nullptr;

    return m_ftFace->glyph;
}

std::vector<ShapedGlyph> FontResource::Shape(std::span<const char32_t> text, uint32_t size)
{
    FT_Set_Pixel_Sizes(m_ftFace, 0, size);

    hb_font_t* hbFont = GetOrCreateHbFont(size);
    if (!hbFont) return {};

    hb_buffer_t* buffer = hb_buffer_create();
    if (!buffer) return {};

    hb_buffer_clear_contents(buffer);
    hb_buffer_add_utf32(
        buffer,
        reinterpret_cast<const uint32_t*>(text.data()),
        (int)text.size(),
        0,
        (int)text.size());

    hb_buffer_guess_segment_properties(buffer);
    hb_shape(hbFont, buffer, nullptr, 0);

    unsigned glyphCount;

    auto infos = hb_buffer_get_glyph_infos(buffer, &glyphCount);
    auto positions = hb_buffer_get_glyph_positions(buffer, &glyphCount);

    std::vector<ShapedGlyph> result;
    result.reserve(glyphCount);

    for (unsigned i = 0; i < glyphCount; ++i)
    {
        ShapedGlyph glyph;

        glyph.glyphIndex = infos[i].codepoint;
        glyph.advanceX = positions[i].x_advance / 64.f;
        glyph.offsetX = positions[i].x_offset / 64.f;
        glyph.offsetY = positions[i].y_offset / 64.f;

        result.push_back(glyph);
    }

    hb_buffer_destroy(buffer);
    return result;
}

hb_font_t* FontResource::GetOrCreateHbFont(uint32_t size)
{
    auto it = m_hbFonts.find(size);
    if (it != m_hbFonts.end())
    {
        return it->second;
    }
    
    hb_font_t* hbFont = hb_ft_font_create_referenced(m_ftFace); // 해당 크기의 hb_font가 없으면 최초 1회만 생성
    if (hbFont)
    {
        hb_ft_font_set_funcs(hbFont);
        hb_font_set_scale(hbFont, size << 6, size << 6); // 중요: 생성 시점에 해당 크기로 스케일을 고정해 둡니다.
        m_hbFonts[size] = hbFont;
    }
    return hbFont;
}


