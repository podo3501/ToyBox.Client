#include "pch.h"
#include "Button.h"
#include "Shared/SerializerIO/SerializerIO.h"
#include "../Traverser/UITraverser.h"

//이 클래스는 당장에는 하는게 없다. container가 할 일을 다 가져갔다. 추후에 이 클래스가 어떻게 될지 지켜보자.
Button::~Button() = default;
Button::Button() :
	m_container{ nullptr }
{}

Button::Button(const Button& o) :
	UIComponent{ o }
{
	ReloadDatas();
}

void Button::ReloadDatas() noexcept
{
	vector<UIComponent*> componentList = GetChildren();
	m_container = componentList[0];
}

unique_ptr<UIComponent> Button::CreateClone() const
{
	return unique_ptr<Button>(new Button(*this));
}

bool Button::ChangeSize(const XMUINT2& size, bool isForce) noexcept
{
	return UITraverser::ChangeSize(m_container, size, isForce);
}

//holdToKeepPressed는 나중에 옵션을 지정하는 변수로 바뀔예정. 버튼은 다양한 버튼들이 존재하기 때문.
bool Button::Setup(const UILayout& layout, unique_ptr<UIComponent> container) noexcept
{
	SetLayout(layout);

	m_container = container.get();
	//m_container->SetStateFlag(StateFlag::Attach | StateFlag::Detach, false);
	AttachComponent(move(container), {});

	return true;
}

void Button::ProcessIO(SerializerIO& serializer)
{
	UIComponent::ProcessIO(serializer);

	if (serializer.IsWrite()) return;
	ReloadDatas();
}