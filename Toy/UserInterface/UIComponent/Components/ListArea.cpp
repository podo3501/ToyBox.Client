#include "pch.h"
#include "ListArea.h"
#include "RenderTexture.h"
#include "ScrollBar.h"
#include "TextureSwitcher.h"
#include "Shared/SerializerIO/SerializerIO.h"
#include "Shared/Utils/StlExt.h"
#include "../Traverser/UITraverser.h"

using namespace UITraverser;

ListArea::~ListArea() = default;
ListArea::ListArea() noexcept :
	m_prototypeContainer{ nullptr },
	m_bgImage{ nullptr },
	m_renderTex{ nullptr },
	m_scrollBar{ nullptr }
{}

ListArea::ListArea(const ListArea& o) noexcept :
	UIComponent{ o },
	m_prototypeContainer{ nullptr },
	m_bgImage{ nullptr },
	m_renderTex{ nullptr },
	m_scrollBar{ nullptr },
	m_scrollPadding{ o.m_scrollPadding }
{
	ReloadDatas();
}

void ListArea::ReloadDatas() noexcept
{
	vector<UIComponent*> componentList = GetChildren();
	m_renderTex = ComponentCast<RenderTexture*>(componentList[0]);
	m_bgImage = m_renderTex->GetRenderedComponent();
	m_prototypeContainer = ComponentCast<TextureSwitcher*>(componentList[1]);
	m_scrollBar = ComponentCast<ScrollBar*>(componentList[2]);
	m_scrollBar->AddScrollChangedCB([this](float ratio) { OnScrollChangedCB(ratio); });
}

unique_ptr<UIComponent> ListArea::CreateClone() const
{
	return unique_ptr<ListArea>(new ListArea(*this));
}

bool ListArea::operator==(const UIComponent& rhs) const noexcept
{
	ReturnIfFalse(UIComponent::operator==(rhs));

	const ListArea* o = static_cast<const ListArea*>(&rhs);
	ReturnIfFalse(EqualComponent(m_prototypeContainer, o->m_prototypeContainer));
	ReturnIfFalse(EqualComponent(m_bgImage, o->m_bgImage));
	ReturnIfFalse(EqualComponent(m_renderTex, o->m_renderTex));
	ReturnIfFalse(EqualComponent(m_scrollBar, o->m_scrollBar));
	ReturnIfFalse(tie(m_scrollPadding) == tie(o->m_scrollPadding));

	return true;
}

bool ListArea::Setup(const UILayout& layout, unique_ptr<UIComponent> bgImage,
	unique_ptr<TextureSwitcher> switcher, unique_ptr<ScrollBar> scrollBar) noexcept
{
	// 추후에 다양한 형태(2줄짜리 ListArea 같은)가 나오면 이 bgImage처럼 새로운 컴포넌트를 만들어서 넣는다.
	// 지금은 Background로 간단하게 했지만 다양한 리스트 형태가 나올수 있다.
	// RenderTexture는 단순히 RenderTexture만 하는 역할로 놔 두자.
	SetLayout(layout);
	
	m_bgImage = bgImage.get();
	m_bgImage->SetName("Background Image");
	m_bgImage->SetStateFlag(StateFlag::LockPosOnResize, true);

	auto renderTex = CreateComponent<RenderTexture>(UILayout{ layout.GetSize() }, move(bgImage));
	m_renderTex = renderTex.get();
	UITraverser::AttachComponent(this, move(renderTex), {});

	m_prototypeContainer = switcher.get();
	m_prototypeContainer->SetName("Prototype Container");
	m_prototypeContainer->SetRegion("ListContainer");
	m_prototypeContainer->SetVisible(false); //Prototype를 만드는 컨테이너이기 때문에 안보이게 셋팅.
	UITraverser::AttachComponent(this, move(switcher), {});

	m_scrollBar = scrollBar.get();
	m_scrollBar->ChangeOrigin(Origin::RightTop);
	m_scrollBar->SetStateFlag(StateFlag::Render, false);
	m_scrollBar->AddScrollChangedCB([this](float ratio) { OnScrollChangedCB(ratio); });
	UITraverser::AttachComponent(this, move(scrollBar), { static_cast<int32_t>(layout.GetSize().x), 0 });

	//자식들은 attach detach가 되는데 prototype은 자식이지만 detach가 안 되어야 한다. 셋팅필요

	return true;
}

bool ListArea::Setup(unique_ptr<UIComponent> bgImage, unique_ptr<TextureSwitcher> switcher, unique_ptr<ScrollBar> scrollBar) noexcept
{
	return Setup({}, move(bgImage), move(switcher), move(scrollBar));
}

bool ListArea::BindSourceInfo(TextureResourceBinder*) noexcept
{
	if (GetSize() == XMUINT2{})
		return UITraverser::ChangeSize(this, GetChildrenBoundsSize(this));

	return UITraverser::ChangeSize(this, GetSize(), true);
}

void ListArea::OnMove(const XMINT2& pos) noexcept
{
	//위치값에 맞는 컨테이너를 찾아서 OnHover를 해 준다.
	pos;
}

bool ListArea::OnWheel(int wheelValue) noexcept
{
	return m_scrollBar->OnWheel(wheelValue);
}

bool ListArea::ChangeScrollBarSizeAndPos(const XMUINT2& size) noexcept
{
	ReturnIfFalse(ChangeSizeY(m_scrollBar, size.y - (m_scrollPadding * 2)));
	XMINT2 pos{ static_cast<int32_t>(size.x - m_scrollPadding), static_cast<int32_t>(m_scrollPadding) };
	return m_scrollBar->ChangeRelativePosition(pos);
}

bool ListArea::ChangeSize(const XMUINT2& size, bool isForce) noexcept
{
	ReturnIfFalse(ChangeSizeX(m_prototypeContainer, size));
	ReturnIfFalse(ChangeScrollBarSizeAndPos(size));
	ReturnIfFalse(UITraverser::ChangeSize(m_renderTex, size, isForce));
	UpdateScrollBar();

	return true;
}

int32_t ListArea::GetContainerHeight() const noexcept
{
	int32_t height{ 0 };
	for (auto container : m_containers)
		height += container->GetSize().y;

	return height;
}

bool ListArea::UpdateScrollBar() noexcept
{
	bool isActiveChange = m_scrollBar->UpdateScrollView(m_renderTex->GetSize().y, GetContainerHeight());
	if (isActiveChange)
		return ResizeContainerForScrollbar();

	return true;
}

XMUINT2 ListArea::GetUsableContentSize() const noexcept
{
	uint32_t padding = m_scrollBar->HasStateFlag(StateFlag::Render) ? 
		m_scrollBar->GetSize().x + m_scrollPadding * 2 : 0;
	XMUINT2 usableSize{ m_prototypeContainer->GetSize() };
	usableSize.x -= padding;

	return usableSize;
}

bool ListArea::ResizeContainerForScrollbar() noexcept
{
	XMUINT2 usableSize(GetUsableContentSize());
	for (auto container : m_containers)
		ReturnIfFalse(UITraverser::ChangeSize(container, usableSize));
	return  true;
}

UIComponent* ListArea::PrepareContainer()
{
	ZoneScoped;

	auto [cloneContainer, cloneContainerPtr] = GetPtrs(UITraverser::Clone(m_prototypeContainer));
	{
		ZoneScopedN("AttachComponent");
		UITraverser::AttachComponent(m_bgImage, move(cloneContainer), {});
	}

	const auto& containerHeight = GetContainerHeight();
	{
		ZoneScopedN("SetInitialState");
		cloneContainerPtr->SetStateFlag(StateFlag::Active, m_containerActiveFlag);
		cloneContainerPtr->ChangeRelativePosition({ 0, containerHeight });
		if (!UITraverser::ChangeSize(cloneContainerPtr, GetUsableContentSize())) return nullptr;
	}
	
	{
		ZoneScopedN("PushToList");
		m_containers.emplace_back(cloneContainerPtr);
	}
	
	{
		ZoneScopedN("UpdateScrollBar");
		if (!UpdateScrollBar()) return nullptr;
	}
	
	return cloneContainerPtr;
}

UIComponent* ListArea::GetContainer(unsigned int idx) const noexcept
{
	if (m_containers.size() <= idx) return nullptr;
	return m_containers[idx];
}

bool ListArea::RemoveContainer(unsigned int idx) noexcept
{
	auto container = GetContainer(idx);
	if (!container) return false;
	
	auto [detach, _] = UITraverser::DetachComponent(container);
	if (!detach) return false;
	m_containers.erase(m_containers.begin() + idx);

	UpdateScrollBar();
	return true;
}

void ListArea::ClearContainers() noexcept
{
	for (auto container : m_containers)
		UITraverser::DetachComponent(container);
	m_containers.clear();
	
	UpdateScrollBar();
}

void ListArea::MoveContainers(int32_t targetPos) noexcept
{
	const auto& containerHeight = m_prototypeContainer->GetSize().y;
	ranges::for_each(m_containers, [idx{ 0u }, targetPos, containerHeight](auto& container) mutable {
		XMINT2 pos = container->GetRelativePosition();
		pos.y = targetPos + ((idx++) * containerHeight);
		container->ChangeRelativePosition(pos);
		});
}

void ListArea::OnScrollChangedCB(float ratio)
{
	const auto& totalContainerHeight = GetContainerHeight();
	auto viewArea = m_renderTex->GetSize().y;
	auto startPos = -static_cast<int32_t>(ratio * (totalContainerHeight - viewArea));

	MoveContainers(startPos);
}

bool ListArea::SetContainerVisible(bool visible) noexcept
{
	for (auto container : m_containers)
		container->SetStateFlag(StateFlag::Active, visible);
	m_containerActiveFlag = visible;

	return true;
}

bool ListArea::EnterToolMode() noexcept
{
	m_prototypeContainer->SetStateFlag(StateFlag::Render, true);
	m_scrollBar->SetStateFlag(StateFlag::Active, true);
	m_scrollBar->RestoreDefault();
	return SetContainerVisible(false);
}

bool ListArea::ExitToolMode() noexcept
{
	m_prototypeContainer->SetStateFlag(StateFlag::Render, false);
	m_scrollBar->SetStateFlag(StateFlag::Active, false);
	SetContainerVisible(true);

	ReturnIfFalse(ResizeContainerForScrollbar());
	return UpdateScrollBar();
}

void ListArea::ProcessIO(SerializerIO& serializer)
{
	UIComponent::ProcessIO(serializer);
	serializer.Process("ScrollPadding", m_scrollPadding);

	if (serializer.IsWrite()) return;
	ReloadDatas();
}