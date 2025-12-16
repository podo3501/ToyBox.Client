#include "pch.h"
#include "PatchTextureLite3.h"
#include "PatchTextureLite1.h"
#include "UserInterface/UIComponent/UIUtility.h"
#include "Shared/SerializerIO/SerializerIO.h"

PatchTextureLite3::~PatchTextureLite3() = default;
PatchTextureLite3::PatchTextureLite3() noexcept :
	PatchTextureLite{ TextureSlice::ThreeH },
	m_dirType{ DirectionType::Horizontal }
{}

PatchTextureLite3::PatchTextureLite3(DirectionType dirType) noexcept
{
	SetDirectionType(dirType);
}

PatchTextureLite3::PatchTextureLite3(const PatchTextureLite3& o) :
	PatchTextureLite{ o },
	m_dirType{ o.m_dirType }
{}

void PatchTextureLite3::SetDirectionType(DirectionType dirType) noexcept
{
	m_dirType = dirType;
	SetTextureSlice(DirTypeToTextureSlice(dirType));
}

unique_ptr<UIComponent> PatchTextureLite3::CreateClone() const
{
	return unique_ptr<PatchTextureLite3>(new PatchTextureLite3(*this));
}

bool PatchTextureLite3::operator==(const UIComponent& rhs) const noexcept
{
	ReturnIfFalse(UIComponent::operator==(rhs));
	const PatchTextureLite3* o = static_cast<const PatchTextureLite3*>(&rhs);

	return (tie(m_dirType) == tie(o->m_dirType));
}

bool PatchTextureLite3::Setup(const XMUINT2& size, DirectionType dirType)
{
	SetLayout(size);
	SetDirectionType(dirType);

	vector<optional<StateFlag::Type>> stateFlags = GetStateFlagsForDirection(dirType);
	for (auto idx : views::iota(0u, 3u))
	{
		auto tex = CreateComponent<PatchTextureLite1>();
		if (auto flag = stateFlags[idx]; flag) tex->SetStateFlag(*flag, true);
		AttachComponent(move(tex), {});
	}
	SetStateFlag(StateFlag::Attach | StateFlag::Detach, false);

	return true;
}

bool PatchTextureLite3::BindSourceInfo(size_t index, const vector<Rectangle>& sources)
{
	ReturnIfFalse(sources.size() == 3);

	for (auto idx : views::iota(0u, sources.size()))
	{
		auto texL = static_cast<PatchTextureLite*>(GetChildComponent(idx));
		texL->BindSourceInfo(index, { sources[idx] });
	}

	return ResizeOrApplyDefault();
}

Rectangle PatchTextureLite3::GetSource() const noexcept
{
	PatchTextureLite1* tex1 = ComponentCast<PatchTextureLite1*>(GetChildComponent(0));
	if (!tex1) return {};

	return tex1->GetSource();
}

void PatchTextureLite3::ProcessIO(SerializerIO& serializer)
{
	UIComponent::ProcessIO(serializer);
	serializer.Process("DirectionType", m_dirType);

	if (serializer.IsWrite()) return;
	SetTextureSlice(DirTypeToTextureSlice(m_dirType));
}