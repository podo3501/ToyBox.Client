#pragma once
#include "TextureInfo.h"

enum class TextureSlice : int;
struct ITextureLoad;
class SerializerIO;
struct TextureSourceInfo : public TextureInfo
{
	~TextureSourceInfo();
	TextureSourceInfo() noexcept;
	TextureSourceInfo(const TextureSourceInfo&) = default; //stl container를 쓴다면 기본 복사 생성자가 생성되어서 원하지 않는 값 복사가 일어날 수 있다.
	TextureSourceInfo(TextureSourceInfo&&) = default;//emplace할때 복사생성자만 있으면 이동이 안됨.
	explicit TextureSourceInfo(const wstring& _filename) noexcept;
	TextureSourceInfo(const wstring& _filename, TextureSlice _texSlice, const vector<Rectangle>& _sources) noexcept;
	TextureSourceInfo& operator=(const TextureSourceInfo&) = default;
	bool operator==(const TextureSourceInfo& o) const noexcept;

	bool LoadResource(ITextureLoad* load);
	void ProcessIO(SerializerIO& serializer);
	inline Rectangle GetSource(size_t index) const noexcept { return (sources.size() > index) ? sources.at(index) : Rectangle{}; }

	using TextureInfo::GetIndex;
	using TextureInfo::GetGfxOffset;

	wstring filename;
	TextureSlice texSlice;
	vector<Rectangle> sources;
};