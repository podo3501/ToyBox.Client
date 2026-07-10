#pragma once
#include "GameClient/Service/Render/Resource/IFontResource.h"
#include "GameClient/Asset/FontAsset.h"
#include <ft2build.h>
#include FT_FREETYPE_H
// 하나의 폰트 파일(.ttf)에 대응하는 런타임 폰트 객체. gpu의 resource는 아님. 즉 펜스로 release 할 필요가 없음.
// FreeType의 FT_Face를 관리

struct FontAsset;
class FreeTypeLibrary;

class FontResource : public IFontResource
{
public:
	~FontResource();
	FontResource();
	virtual bool IsReady() const noexcept override { return m_ready; }

	bool Initialize(FreeTypeLibrary& ftLibrary, std::shared_ptr<FontAsset> asset);
	FT_GlyphSlot GetGlyphSlot(char32_t codepoint, uint32_t fontSize) const;
	void MarkReady() noexcept { m_ready = true; }

private:
	FT_Face m_ftFace{ nullptr };

	bool m_ready{ false };
	std::shared_ptr<FontAsset> m_asset;
};
