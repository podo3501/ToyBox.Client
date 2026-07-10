#include "pch.h"
#include "FontResource.h"
#include "Core/FreeTypeLibrary.h"

FontResource::~FontResource()
{
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

	m_asset = std::move(asset);
	MarkReady();

	return true;
}

FT_GlyphSlot FontResource::GetGlyphSlot(char32_t codepoint, uint32_t fontSize) const
{
    FT_Set_Pixel_Sizes(m_ftFace, 0, fontSize);
    if (FT_Load_Char(m_ftFace, codepoint, FT_LOAD_RENDER))
        return nullptr;

    return m_ftFace->glyph;
}


