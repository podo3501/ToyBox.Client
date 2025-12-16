#pragma once
#include "../PatchTexture.h"

struct TextureSourceInfo;
class PatchTextureLite : public PatchTexture
{
public:
	~PatchTextureLite();	
	virtual bool BindSourceInfo(size_t index, const vector<Rectangle>& sources) = 0;
	virtual void SetIndexedSource(size_t, const vector<Rectangle>&) noexcept;

protected:
	using PatchTexture::PatchTexture;
	vector<Rectangle> GetSources(const vector<Rectangle>& srcs, size_t index);
	bool ForEach(predicate<PatchTextureLite*, size_t> auto&& Each);

	PatchTextureLite();
	PatchTextureLite(const PatchTextureLite& other) noexcept;
};

unique_ptr<PatchTextureLite> CreatePatchTextureLite(TextureSlice texSlice, const XMUINT2& size);

inline vector<Rectangle> GetTripleAt(const vector<Rectangle>& srcs, size_t idx) noexcept
{
	size_t base = idx * 3;
	return { srcs[base + 0], srcs[base + 1], srcs[base + 2] };
}