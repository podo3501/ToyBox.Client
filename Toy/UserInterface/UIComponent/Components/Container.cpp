#include "pch.h"
#include "Container.h"
#include "Locator/InputLocator.h"
#include "../Traverser/UITraverser.h"
#include "Shared/SerializerIO/SerializerIO.h"
#include "Shared/Utils/GeometryExt.h"
#include "Shared/Utils/StlExt.h"

using enum InteractState;

Container::~Container() = default;
Container::Container() noexcept :
	m_state{ nullopt }
{}

Container::Container(const Container& o) :
	UIComponent{ o },
	m_state{ o.m_state }
{
	ReloadDatas();
}

void Container::ReloadDatas() noexcept
{
	vector<UIComponent*> componentList = GetChildren();
	m_textures.emplace(Normal, componentList[0]);		//여기에 순서가 잘못되면 안된다.
	m_textures.emplace(Hovered, componentList[1]);
	m_textures.emplace(Pressed, componentList[2]);
}

unique_ptr<UIComponent> Container::CreateClone() const
{
	return unique_ptr<Container>(new Container(*this));
}

bool Container::operator==(const UIComponent& rhs) const noexcept
{
	ReturnIfFalse(UIComponent::operator==(rhs));
	const Container* o = static_cast<const Container*>(&rhs);

	//return ranges::all_of(m_textures, [o](const auto& pair) {
	//	auto state = pair.first;
	//	return pair.second->GetName() == o->m_textures.at(state)->GetName();
	//	});
	return CompareAssoc(m_textures, o->m_textures);
}

bool Container::BindSourceInfo(TextureResourceBinder*) noexcept
{
	SetState(Normal);
	return true;
}

void Container::ClearInteraction() noexcept
{ 
	if (m_state && m_state == InteractState::Hovered)
		SetState(InteractState::Normal);
}

bool Container::ChangeSize(const XMUINT2& size, bool isForce) noexcept
{
	return ranges::all_of(GetChildren(), [&size, isForce](const auto& component) {
			return UITraverser::ChangeSize(component, size, isForce); });
}

void Container::AttachComponent(InteractState state, unique_ptr<UIComponent>&& component) noexcept
{
	m_textures.emplace(state, component.get());
	UIComponent::AttachComponent(move(component), {});
}

inline static void SetActiveStateFlag(bool condition, UIComponent* component) noexcept
{
	component->SetStateFlag(StateFlag::Active, condition);
}

bool Container::Setup(const UILayout& layout, 
	map<InteractState, unique_ptr<UIComponent>> patchTexList) noexcept
{
	SetLayout(layout);

	for (auto& pTex : patchTexList)
	{
		SetActiveStateFlag(pTex.first == Normal, pTex.second.get());
		AttachComponent(pTex.first, move(pTex.second));
	}

	return true;
}

void Container::SetState(InteractState curState) noexcept
{
	if (m_state == curState) return;

	for (auto& [state, tex] : m_textures)
		SetActiveStateFlag(state == curState, tex);

	m_state = curState;
}

void Container::ProcessIO(SerializerIO& serializer)
{
	UIComponent::ProcessIO(serializer);

	if (serializer.IsWrite()) return;
	ReloadDatas();
}