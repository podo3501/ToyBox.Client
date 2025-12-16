#pragma once
#include "TextureInfo.h"

struct ITextureLoad;
class SerializerIO;
struct TextureFontInfo : public TextureInfo
{
	~TextureFontInfo();
	TextureFontInfo() noexcept;
	TextureFontInfo(const TextureFontInfo& other) noexcept;
	explicit TextureFontInfo(const wstring& _filename) noexcept;
	bool operator==(const TextureFontInfo& o) const noexcept;

	bool LoadResource(ITextureLoad* load);
	void ProcessIO(SerializerIO& serializer);

	using TextureInfo::GetIndex;

	wstring filename;
};