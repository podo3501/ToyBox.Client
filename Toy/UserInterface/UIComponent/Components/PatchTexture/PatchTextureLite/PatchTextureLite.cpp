#include "pch.h"
#include "PatchTextureLite.h"
#include "PatchTextureLite1.h"
#include "PatchTextureLite3.h"
#include "PatchTextureLite9.h"

PatchTextureLite::~PatchTextureLite() = default;
PatchTextureLite::PatchTextureLite() = default;
PatchTextureLite::PatchTextureLite(const PatchTextureLite& other) noexcept :
	PatchTexture{ other }
{}

vector<Rectangle> PatchTextureLite::GetSources(const vector<Rectangle>& srcs, size_t index)
{
	switch (GetTypeID())
	{
	case ComponentID::PatchTextureLite3: return vector<Rectangle>{ srcs[index] };
	case ComponentID::PatchTextureLite9: return GetTripleAt(srcs, index);
	}
	return {};
}

void PatchTextureLite::SetIndexedSource(size_t index, const vector<Rectangle>& sources) noexcept
{
	ForEach([this, index, &sources](PatchTextureLite* tex, size_t idx) {
		tex->SetIndexedSource(index, GetSources(sources, idx));
		return true;
		});
}

bool PatchTextureLite::ForEach(predicate<PatchTextureLite*, size_t> auto&& Each)
{
	const auto& components = GetChildren();
	size_t size = components.size();
	ReturnIfFalse(size >= 3);

	for (size_t n : views::iota(0u, size))
		if (!Each(static_cast<PatchTextureLite*>(components[n]), n)) return false;

	return true;
}

unique_ptr<PatchTextureLite> CreatePatchTextureLite(TextureSlice texSlice, const XMUINT2& size)
{
	switch (texSlice) {
	case TextureSlice::One: return CreateComponent<PatchTextureLite1>(size);
	case TextureSlice::ThreeH: return CreateComponent<PatchTextureLite3>(size, DirectionType::Horizontal);
	case TextureSlice::ThreeV: return CreateComponent<PatchTextureLite3>(size, DirectionType::Vertical);
	case TextureSlice::Nine: return CreateComponent<PatchTextureLite9>(size);
	}
	return nullptr;
}