#include "pch.h"
#include "UITransform.h"
#include "UILayout.h"
#include "Shared/SerializerIO/SerializerIO.h"
#include "Shared/Utils/GeometryExt.h"

UITransform::UITransform() = default;
bool UITransform::operator==(const UITransform& o) const noexcept
{
	if (m_relativePosition != o.m_relativePosition) return false;
	ReturnIfFalse(CompareEpsilon(m_ratio, o.m_ratio));
	
	return true;
}

void UITransform::Clear() noexcept
{
	m_ratio = {};
	m_absolutePosition = {};
	m_relativePosition = {};
}

XMINT2 UITransform::GetUpdatedPosition(const UILayout& layout, const XMINT2& parentPos) noexcept
{
	m_absolutePosition = layout.GetPosition(parentPos);
	return m_absolutePosition;
}

static inline Vector2 CalcRatio(const XMUINT2& size, const XMINT2& pos) noexcept
{
	return (size == XMUINT2{}) ? Vector2{ 0.f, 0.f } : Vector2{
		static_cast<float>(pos.x) / static_cast<float>(size.x),
		static_cast<float>(pos.y) / static_cast<float>(size.y)
	};
}

void UITransform::ChangeRelativePosition(const XMUINT2& size, const XMINT2& relativePos) noexcept
{
	m_ratio = CalcRatio(size, relativePos);
	m_relativePosition = relativePos;
}

void UITransform::AdjustPosition(const XMUINT2& size, bool lockPosition) noexcept
{
	if (!lockPosition) //lockPosition의 기본값은 false이다.
	{
		m_relativePosition = {
			static_cast<int32_t>(static_cast<float>(size.x) * m_ratio.x),
			static_cast<int32_t>(static_cast<float>(size.y) * m_ratio.y) };
	}
	else
		m_ratio = CalcRatio(size, m_relativePosition);
}

void UITransform::ProcessIO(SerializerIO& serializer)
{
	serializer.Process("Ratio", m_ratio);
	serializer.Process("RelativePosition", m_relativePosition);
}