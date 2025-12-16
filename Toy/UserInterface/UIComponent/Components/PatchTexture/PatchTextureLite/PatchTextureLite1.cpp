#include "pch.h"
#include "PatchTextureLite1.h"

PatchTextureLite1::~PatchTextureLite1() = default;
PatchTextureLite1::PatchTextureLite1() :
	PatchTextureLite{ TextureSlice::One }
{
	m_coord.SetOwner(this);
}

PatchTextureLite1::PatchTextureLite1(const PatchTextureLite1& o) :
	PatchTextureLite{ o },
	m_coord{ o.m_coord }
{
	m_coord.SetOwner(this); //생성될때 PatchTextureCoord m_coord{ this }; 를 실행하지 않기 때문에 명시적으로 넣어준다.
}

unique_ptr<UIComponent> PatchTextureLite1::CreateClone() const
{
	return unique_ptr<PatchTextureLite1>(new PatchTextureLite1(*this));
}

bool PatchTextureLite1::operator==(const UIComponent& rhs) const noexcept
{
	ReturnIfFalse(UIComponent::operator==(rhs));
	const PatchTextureLite1* o = static_cast<const PatchTextureLite1*>(&rhs);

	return (tie(m_coord) == tie(o->m_coord));
}

bool PatchTextureLite1::Setup(const XMUINT2& size)
{
	SetLayout(size);
	return true;
}

bool PatchTextureLite1::BindSourceInfo(size_t index, const vector<Rectangle>& sources)
{
	SetIndexedSource(index, sources);
	if (GetSize() == XMUINT2{})
		SetLayout(GetSourceSize(m_coord));

	return true;
}

void PatchTextureLite1::SetIndexedSource(size_t index, const vector<Rectangle>& source) noexcept 
{ 
	AssertMsg(source.size() == 1, "The source size must be 1");
	m_coord.SetIndexedSource(index, source[0]);
}