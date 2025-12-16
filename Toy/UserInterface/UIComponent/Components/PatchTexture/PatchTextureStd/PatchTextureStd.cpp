#include "pch.h"
#include "PatchTextureStd.h"
#include "PatchTextureStd1.h"
#include "PatchTextureStd3.h"
#include "PatchTextureStd9.h"
#include "UserInterface/TextureResourceBinder/TextureResourceBinder.h"

PatchTextureStd::~PatchTextureStd() = default;
PatchTextureStd::PatchTextureStd() = default;
PatchTextureStd::PatchTextureStd(const PatchTextureStd& other) noexcept :
	PatchTexture{ other }
{}

const string& PatchTextureStd::GetBindKey() const noexcept
{
	PatchTextureStd* child = static_cast<PatchTextureStd*>(GetChildComponent(0));
	return child->GetBindKey();
}

bool PatchTextureStd::BindSourceInfo(TextureResourceBinder*) noexcept
{
	return ResizeOrApplyDefault();
}

bool PatchTextureStd::ChangeBindKey(TextureResourceBinder* resBinder, const string& key) noexcept
{
    if (auto infoRef = resBinder->GetTextureSourceInfo(key); infoRef)
    {
        ChangeBindKeyWithIndex(key, *infoRef, 0);
        return FitToTextureSource();
    }

    return false;
}

bool PatchTextureStd::ChangeBindKeyWithIndex(const string& key, const TextureSourceInfo& info, size_t sourceIndex) noexcept
{
	for (size_t index : views::iota(0u, 3u))
	{
		PatchTextureStd* child = static_cast<PatchTextureStd*>(GetChildComponent(index));
		ReturnIfFalse(child->ChangeBindKeyWithIndex(key, info, sourceIndex * 3 + index));
	}

	return true;
}

//////////////////////////////////////////////////////////

unique_ptr<PatchTextureStd> CreatePatchTexture(const UILayout& layout, TextureSlice texSlice, const string& bindKey)
{
	UILayout ltLayout(layout.GetSize());
	switch (texSlice) {
	case TextureSlice::One: return CreateComponent<PatchTextureStd1>(ltLayout, bindKey);
	case TextureSlice::ThreeH: return CreateComponent<PatchTextureStd3>(ltLayout, DirectionType::Horizontal, bindKey);
	case TextureSlice::ThreeV: return CreateComponent<PatchTextureStd3>(ltLayout, DirectionType::Vertical, bindKey);
	case TextureSlice::Nine: return CreateComponent<PatchTextureStd9>(ltLayout, bindKey);
	}
	return nullptr;
}