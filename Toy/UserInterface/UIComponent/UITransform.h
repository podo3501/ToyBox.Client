#pragma once

class UILayout;
class SerializerIO;

class UITransform
{
public:
	UITransform();
	bool operator==(const UITransform& o) const noexcept;

	void Clear() noexcept;
	XMINT2 GetUpdatedPosition(const UILayout& layout, const XMINT2& parentPos) noexcept;
	void ChangeRelativePosition(const XMUINT2& size, const XMINT2& relativePos) noexcept;
	void AdjustPosition(const XMUINT2& size, bool lockPosition) noexcept;
	void ProcessIO(SerializerIO& serializer);

	inline const Vector2& GetRatio() const noexcept { return m_ratio; }
	inline const XMINT2& GetRelativePosition() const noexcept { return m_relativePosition; }
	inline const XMINT2& GetAbsolutePosition() const noexcept { return m_absolutePosition; }

private:
	Vector2 m_ratio{};
	XMINT2 m_relativePosition{}; //부모의 관계에 비례해서 상대적인 좌표
	XMINT2 m_absolutePosition{}; //LeftTop이며, 계산되어져서 실제인 좌표
};
