#include "pch.h"
#include "UIComponent.h"
#include "Shared/Utils/StlExt.h"
#include "Shared/SerializerIO/SerializerIO.h"
#include "../SerializerIO/ClassSerializeIO.h"

UIComponent::~UIComponent() = default;
UIComponent::UIComponent() :
	m_root{ this },
	m_layout{ XMUINT2{}, Origin::LeftTop }
{}

UIComponent::UIComponent(const string& name, const UILayout& layout) noexcept :
	m_root{ this },
	m_name{ name },
	m_layout{ layout }
{}

//상속받은 곳에서만 복사생성자를 호출할 수 있다.
UIComponent::UIComponent(const UIComponent& other)
{
	m_root = other.m_root;
	m_name = other.m_name;
	m_layout = other.m_layout;
	m_region = other.m_region;
	m_stateFlag = other.m_stateFlag;
	m_renderTraversal = other.m_renderTraversal;
	m_transform = other.m_transform;

	ranges::transform(other.m_children, back_inserter(m_children), [this, &other](const auto& child) {
		auto component = child->CreateClone();
		component->SetParent(this);
		return move(component);
	});
}

bool UIComponent::EqualComponent(const UIComponent* lhs, const UIComponent* rhs) const noexcept
{
	if (lhs == nullptr && rhs == nullptr) return true;
	if (lhs == nullptr || rhs == nullptr) return false;
	if (lhs->m_name != rhs->m_name) return false;

	return true;
}

bool UIComponent::operator==(const UIComponent& o) const noexcept
{
	if (GetTypeID() != o.GetTypeID()) return false;

	ReturnIfFalse(EqualComponent(m_root, o.m_root));
	ReturnIfFalse(tie(m_name, m_layout, m_region, m_stateFlag, m_transform, m_renderTraversal) ==
		tie(o.m_name, o.m_layout, o.m_region, o.m_stateFlag, o.m_transform, o.m_renderTraversal));
	ReturnIfFalse(EqualComponent(m_parent, o.m_parent));
	ReturnIfFalse(m_children.size() == o.m_children.size());
	ReturnIfFalse(CompareSeq(m_children, o.m_children));

	return true;
}

void UIComponent::Unlink() noexcept
{
	m_root = this;
	m_parent = nullptr;
	m_transform.Clear();
}

unique_ptr<UIComponent> UIComponent::Clone() const 
{ 
	auto clone = CreateClone();
	clone->Unlink();
	return clone;
}

unique_ptr<UIComponent> UIComponent::AttachComponent(unique_ptr<UIComponent> child, const XMINT2& relativePos) noexcept
{
	if (!HasStateFlag(StateFlag::Attach))
		return move(child);

	child->SetParent(this);
	child->m_transform.ChangeRelativePosition(
		m_layout.GetSize(), relativePos);
	m_children.emplace_back(move(child));

	return nullptr;
}

pair<unique_ptr<UIComponent>, UIComponent*> UIComponent::DetachComponent() noexcept
{
	if (!m_parent || !m_parent->HasStateFlag(StateFlag::Detach))
		return {};

	auto& children = m_parent->m_children;
	auto it = std::ranges::find_if(children, [this](auto& c) { return c.get() == this; });
	if (it == children.end())
		return {};

	auto detached = move(*it);
	UIComponent* parent = m_parent;

	children.erase(it);
	detached->Unlink();

	return { move(detached), parent };
}

void UIComponent::SetChildrenStateFlag(StateFlag::Type flag, bool enabled) noexcept
{
	ranges::for_each(m_children, [this, flag, enabled](auto& child) {
		child->SetStateFlag(flag, enabled);
		});
}

bool UIComponent::ResizeAndAdjustPos(const XMUINT2& size) noexcept
{
	ranges::for_each(m_children, [this, &size](auto& child) {
		child->GetTransform().AdjustPosition(size, HasStateFlag(StateFlag::LockPosOnResize));
		});

	SetSize(size);
	return true;
}

bool UIComponent::ChangePosition(size_t index, const XMUINT2& size, const XMINT2& relativePos) noexcept
{
	if (index >= m_children.size()) return false;
	m_children[index]->GetTransform().ChangeRelativePosition(size, relativePos);
	return true;
}

bool UIComponent::ChangeRelativePosition(const XMINT2& relativePos) noexcept
{
	if (!m_parent) return false;
	m_transform.ChangeRelativePosition(m_parent->GetSize(), relativePos);
	return true;
}

void UIComponent::ProcessIO(SerializerIO& serializer)
{
	serializer.Process("Name", m_name);
	serializer.Process("Layout", m_layout);
	serializer.Process("Transform", m_transform);
	serializer.Process("Region", m_region);
	serializer.Process("StateFlag", m_stateFlag);
	serializer.Process("RenderSearch", m_renderTraversal);
	serializer.Process("Children", m_children);
	
	if (serializer.IsWrite()) return;
	ranges::for_each(m_children, [this](auto& child) {
		child->SetParent(this);
		});
}

Rectangle UIComponent::GetArea() const noexcept
{
	const XMINT2& curLeftTop = GetLeftTop();
	const XMUINT2& curSize = GetSize();
	return Rectangle(curLeftTop.x, curLeftTop.y, curSize.x, curSize.y);
}

UIComponent* UIComponent::GetChildComponent(size_t index) const noexcept
{
	if (index >= m_children.size()) return nullptr;
	return m_children[index].get();
}

vector<UIComponent*> UIComponent::GetChildren() const noexcept
{
	vector<UIComponent*> componentList;
	componentList.reserve(m_children.size());

	for (const auto& child : m_children)
	{
		if (child)
			componentList.push_back(child.get());
	}

	return componentList;
}
