#pragma once
#include "GameClient/Service/Render/Resource/IResource.h"
#include "GameClient/Asset/BinaryAsset.h"
#include "TextSystem/TextTypes.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_MODULE_H
#include <harfbuzz/hb.h>
#include <harfbuzz/hb-ft.h>

class FreeTypeLibrary;

// 하나의 폰트 파일(.ttf)에 대응하는 런타임 폰트 객체. gpu의 resource는 아님. 즉 펜스로 release 할 필요가 없음. FreeType의 FT_Face를 관리
class FontResource : public IResource
{
public:
	virtual ~FontResource() override;
	FontResource();
	virtual bool IsReady() const noexcept override { return m_ready; }

	bool Initialize(FreeTypeLibrary& ftLibrary, std::shared_ptr<BinaryAsset> asset);
	FT_GlyphSlot GetGlyphSlot(uint32_t glyphIndex, uint32_t size) const;

	std::vector<ShapedGlyph> Shape(std::span<const char32_t> text, uint32_t size);
	FT_Face GetFtFace() { return m_ftFace; }

	float GetLineHeight(uint32_t size) const;
	float GetAscent(uint32_t size) const;
	float GetUnderlineThickness(uint32_t size) const;
	float GetUnderlineOffset(uint32_t size) const;

private:
	hb_font_t* GetOrCreateHbFont(uint32_t size);

	FT_Face m_ftFace{ nullptr };
	std::unordered_map<uint32_t, hb_font_t*> m_hbFonts; // 크기별 HarfBuzz 폰트 캐시

	bool m_ready{ false };
	std::shared_ptr<BinaryAsset> m_asset;
};
