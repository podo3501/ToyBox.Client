#include "pch.h"
#include "FontResource.h"
#include "Core/FreeTypeLibrary.h"

FontResource::~FontResource()
{
    if (m_hbBuffer)
    {
        hb_buffer_destroy(m_hbBuffer);
        m_hbBuffer = nullptr;
    }

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
		&m_ftFace
	);
	if (error)
		return false;

    m_hbBuffer = hb_buffer_create();
    if (!m_hbBuffer)
        return false;

	m_asset = std::move(asset);
	MarkReady();

	return true;
}

FT_GlyphSlot FontResource::GetGlyphSlot(char32_t codepoint, uint32_t fontSize) const
{
    FT_Set_Pixel_Sizes(m_ftFace, 0, fontSize);
    FT_UInt glyphIndex = FT_Get_Char_Index(m_ftFace, codepoint);

    if (FT_Load_Glyph(m_ftFace, glyphIndex, FT_LOAD_DEFAULT))
        return nullptr;

    if (FT_Render_Glyph(m_ftFace->glyph, FT_RENDER_MODE_NORMAL))
        return nullptr;

    return m_ftFace->glyph;
}

FT_GlyphSlot FontResource::GetGlyphSlotByGlyphIndex(uint32_t glyphIndex, uint32_t size) const
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
    if (!hbFont || !m_hbBuffer) return {};

    hb_ft_font_changed(hbFont);
    hb_buffer_clear_contents(m_hbBuffer);
    hb_buffer_add_utf32(
        m_hbBuffer,
        reinterpret_cast<const uint32_t*>(text.data()),
        (int)text.size(),
        0,
        (int)text.size());

    hb_buffer_guess_segment_properties(m_hbBuffer);
    hb_shape(hbFont, m_hbBuffer, nullptr, 0);

    unsigned glyphCount;

    auto infos = hb_buffer_get_glyph_infos(m_hbBuffer, &glyphCount);
    auto positions = hb_buffer_get_glyph_positions(m_hbBuffer, &glyphCount);

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


