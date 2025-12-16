#include "pch.h"
#include "PatchTextureCoord.h"
#include "IRenderer.h"
#include "UserInterface/UIComponent/UIComponent.h"
#include "Shared/Utils/GeometryExt.h"

PatchTextureCoord::~PatchTextureCoord() = default;
PatchTextureCoord::PatchTextureCoord() noexcept :
	m_component{ nullptr }
{}

PatchTextureCoord::PatchTextureCoord(const PatchTextureCoord& o) noexcept :
	m_component{ nullptr },
	m_index{ o.m_index },
	m_source{ o.m_source }
{}

bool PatchTextureCoord::operator==(const PatchTextureCoord& rhs) const noexcept
{
	auto result = (tie(m_index, m_source) == tie(rhs.m_index, rhs.m_source));
	Assert(result);

	return result;
}

void PatchTextureCoord::Render(ITextureRender* render) const
{
	RECT source = RectangleToRect(m_source);
	RECT dest = RectangleToRect(m_component->GetArea());

	render->Render(*m_index, dest, &source);
}

void PatchTextureCoord::SetIndexedSource(size_t index, const Rectangle& source) noexcept
{
	m_index = index;
	m_source = source;
}

bool PatchTextureCoord::FitToTextureSource() noexcept
{
	Assert(!m_source.IsEmpty()); //좌표가 없는건데 아마 로딩하고나서 Bind를 안해 줬거나 생성시 안해줬거나 attach가 안돼서 bind가 안됐을 가능성이 있다.
	if (m_source.IsEmpty()) return false;

	m_component->SetSize(GetSizeFromRectangle(m_source));
	return true;
}

//utility
XMUINT2 GetSourceSize(const PatchTextureCoord& coord) noexcept
{
	return GetSizeFromRectangle(coord.GetSource());
}
