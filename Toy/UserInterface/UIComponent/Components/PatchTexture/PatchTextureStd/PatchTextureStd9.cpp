#include "pch.h"
#include "PatchTextureStd9.h"
#include "PatchTextureStd3.h"
#include "UserInterface/TextureResourceBinder/TextureResourceBinder.h"
#include "UserInterface/UIComponent/UIUtility.h"

PatchTextureStd9::~PatchTextureStd9() = default;
PatchTextureStd9::PatchTextureStd9() noexcept :
	PatchTextureStd{ TextureSlice::Nine }
{}

PatchTextureStd9::PatchTextureStd9(const PatchTextureStd9& o) noexcept :
	PatchTextureStd{ o }
{}

unique_ptr<UIComponent> PatchTextureStd9::CreateClone() const
{
	return unique_ptr<PatchTextureStd9>(new PatchTextureStd9(*this));
}

bool PatchTextureStd9::Setup(const UILayout& layout, const string& bindKey)
{
	SetLayout(layout);

	vector<optional<StateFlag::Type>> stateFlags = GetStateFlagsForDirection(DirectionType::Vertical);
	for (size_t idx : views::iota(0u, 3u))
	{
		auto tex3 = CreateComponent<PatchTextureStd3>(DirectionType::Horizontal, bindKey, idx);
		if (auto flag = stateFlags[idx]; flag) tex3->SetStateFlag(*flag, true);
		AttachComponent(move(tex3), {});
	}
	SetStateFlag(StateFlag::Attach | StateFlag::Detach, false);

	return true;
}