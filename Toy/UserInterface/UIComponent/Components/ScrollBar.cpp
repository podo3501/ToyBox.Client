#include "pch.h"
#include "ScrollBar.h"
#include "UserInterface/UIComponent/Traverser/UITraverser.h"
#include "PatchTexture/PatchTextureStd/PatchTextureStd3.h"
#include "TextureSwitcher.h"
#include "Shared/SerializerIO/SerializerIO.h"
#include "Shared/Utils/GeometryExt.h"

using namespace UITraverser;

ScrollBar::~ScrollBar() = default;
ScrollBar::ScrollBar() :
	m_scrollTrack{ nullptr },
	m_scrollButton{ nullptr }
{
	SetRenderTraversal(RenderTraversal::DFS);
}

ScrollBar::ScrollBar(const ScrollBar& other) :
	UIComponent{ other },
	m_scrollTrack{ nullptr },
	m_scrollButton{ nullptr }
{
	ReloadDatas();
}

void ScrollBar::ReloadDatas() noexcept
{
	vector<UIComponent*> componentList = GetChildren();
	m_scrollTrack = ComponentCast<PatchTextureStd3*>(componentList[0]);
	m_scrollButton = ComponentCast<TextureSwitcher*>(componentList[1]);
}

bool ScrollBar::operator==(const UIComponent& rhs) const noexcept
{
	ReturnIfFalse(UIComponent::operator==(rhs));

	const ScrollBar* o = static_cast<const ScrollBar*>(&rhs);
	ReturnIfFalse(EqualComponent(m_scrollTrack, o->m_scrollTrack));
	ReturnIfFalse(EqualComponent(m_scrollButton, o->m_scrollButton));

	return true;
}

unique_ptr<UIComponent> ScrollBar::CreateClone() const
{
	return unique_ptr<ScrollBar>(new ScrollBar(*this));
}

void ScrollBar::RestoreDefault() noexcept
{
	SetScrollContainerSize(0.5f);
	SetPositionRatio(0.f);
	m_bounded.Reset();
}

bool ScrollBar::Setup(const UILayout& layout,
	unique_ptr<PatchTextureStd3> scrollTrack,
	unique_ptr<TextureSwitcher> scrollButton)
{
	SetLayout(layout);

	m_scrollTrack = scrollTrack.get();
	AttachComponent(move(scrollTrack), {});

	m_scrollButton = scrollButton.get();
	AttachComponent(move(scrollButton), {});

	DirectionType dirType = m_scrollTrack->GetDirectionType();
	StateFlag::Type flag = (dirType == DirectionType::Vertical) ? StateFlag::X_SizeLocked : StateFlag::Y_SizeLocked;
	SetStateFlag(flag, true);

	return true;
}

bool ScrollBar::Setup(unique_ptr<PatchTextureStd3> scrollTrack, unique_ptr<TextureSwitcher> scrollButton)
{
	return Setup({}, move(scrollTrack), move(scrollButton));
}

bool ScrollBar::BindSourceInfo(TextureResourceBinder*) noexcept
{
	if (GetSize() == XMUINT2{})
		SetSize(GetChildrenBoundsSize(this));

	SetScrollContainerSize(0.5f);	//기본값
	return true;
}

InputResult ScrollBar::OnPress(const XMINT2& position) noexcept
{
	if (Contains(m_scrollButton->GetArea(), position))
		m_pressMousePosY = position.y;
	m_pressButtonPos = m_scrollButton->GetRelativePosition();

	return InputResult::Consumed;
}

void ScrollBar::OnHold(const XMINT2& position, bool) noexcept
{
	auto maxRange = GetMaxScrollRange<int32_t>();
	int32_t curY{ 0 };

	if (m_pressMousePosY)
	{
		int32_t moved = position.y - m_pressMousePosY.value();
		curY = std::clamp(m_pressButtonPos.y + moved, 0, maxRange);
	}
	else
	{
		int32_t localY = position.y - GetLeftTop().y;
		int32_t halfButtonHeight = static_cast<int32_t>(m_scrollButton->GetArea().height / 2);
		curY = std::clamp(localY - halfButtonHeight, 0, maxRange);
	}

	m_scrollButton->ChangeRelativePosition({ m_pressButtonPos.x, curY });

	auto ratio = static_cast<float>(curY) / static_cast<float>(maxRange);
	m_bounded.SetPositionRatio(ratio);
	m_onScrollChangedCB(ratio);

	m_scrollButton->ChangeState(InteractState::Pressed);
}

void ScrollBar::OnRelease(bool) noexcept
{
	m_pressMousePosY = nullopt;
	m_scrollButton->ChangeState(InteractState::Normal, true);
}

bool ScrollBar::OnWheel(int wheelValue) noexcept
{
	m_bounded.SetValue(wheelValue);
	return true;
}

bool ScrollBar::Update(const DX::StepTimer& timer) noexcept
{
	if (!m_bounded.UpdateLerpedValue(timer)) return true;

	auto posRatio = m_bounded.GetPositionRatio();
	ApplyScrollButtonPosition(posRatio);
	m_onScrollChangedCB(posRatio);

	return true;
}

void ScrollBar::ProcessIO(SerializerIO& serializer)
{
	UIComponent::ProcessIO(serializer);

	if (serializer.IsWrite()) return;
	ReloadDatas();
}

template<typename ReturnType>
ReturnType ScrollBar::GetMaxScrollRange() const noexcept
{
	const XMUINT2& trackSize = m_scrollTrack->GetSize();
	const XMUINT2& containerSize = m_scrollButton->GetSize();
	return static_cast<ReturnType>(trackSize.y - containerSize.y);
}

//이 함수는 세로만 적용돼 있다.
bool ScrollBar::ChangeSize(const XMUINT2& newSize, bool isForce) noexcept
{
	float sizeRatio = static_cast<float>(newSize.y) / static_cast<float>(m_scrollTrack->GetSize().y);
	ReturnIfFalse(UITraverser::ChangeSize(m_scrollTrack, newSize, isForce));

	//크기가 바뀌면 상대적으로 버튼 크기가 정해지기 때문에 조정되어야 한다.
	uint32_t btnHeight = static_cast<uint32_t>(static_cast<float>(m_scrollButton->GetSize().y) * sizeRatio);
	auto btnSize = XMUINT2{ newSize.x, btnHeight };
	UITraverser::ChangeSize(m_scrollButton, btnSize, isForce);

	return true;
}

bool ScrollBar::UpdateScrollView(uint32_t viewArea, uint32_t contentSize) noexcept
{
	if(GetToolMode())
	{
		SetScrollContainerSize(0.5f);
		return false;
	}

	if (contentSize <= viewArea)
		return SetStateFlag(StateFlag::Active, false);

	bool changeFlag = SetStateFlag(StateFlag::Active, true);
	if (int height = viewArea - contentSize; height < 0)
		m_bounded.SetBounds(height, 0, 15);

	SetScrollContainerSize(static_cast<float>(viewArea) / static_cast<float>(contentSize));
	return changeFlag;
}

void ScrollBar::SetScrollContainerSize(float ratio) noexcept
{
	DirectionType dirType = m_scrollTrack->GetDirectionType();
	const auto& area = m_scrollTrack->GetArea();
	XMUINT2 ratioSize = GetSizeFromRectangle(area);

	switch (dirType) {
	case DirectionType::Horizontal: ratioSize.x = static_cast<uint32_t>(area.width * ratio); break;
	case DirectionType::Vertical: ratioSize.y = static_cast<uint32_t>(area.height * ratio); break;
	}

	UITraverser::ChangeSize(m_scrollButton, ratioSize);
}

void ScrollBar::ApplyScrollButtonPosition(float positionRatio) noexcept
{
	positionRatio = clamp(positionRatio, 0.f, 1.f);

	auto relativePosY = static_cast<int32_t>(GetMaxScrollRange<float>() * positionRatio);
	const auto& curPos = m_scrollButton->GetRelativePosition();
	m_scrollButton->ChangeRelativePosition({ curPos.x, relativePosY });
}

void ScrollBar::SetPositionRatio(float positionRatio) noexcept
{
	ApplyScrollButtonPosition(positionRatio);
	m_bounded.SetPositionRatio(positionRatio);
}