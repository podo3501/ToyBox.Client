#include "pch.h"
#include "UITraverser.h"
#include "../UIComponent.h"
#include "Toy/UserInterface/UIComponent/Traverser/BaseTraverser.h"
#include "Toy/UserInterface/UIComponent/Traverser/DerivedTraverser.h"
#include "Toy/UserInterface/UIComponent/Traverser/NameTraverser.h"

namespace UITraverser
{
	BaseTraverser g_baseTraverser{};
	DerivedTraverser g_derivedTraverser{};
	NameTraverser g_nameTraverser{};
	
	////////////////////////////////////////////////////////////////
	// BaseTraverser

	XMUINT2 GetChildrenBoundsSize(UIComponent* c) noexcept { 
		return g_baseTraverser.GetChildrenBoundsSize(c); }
	vector<UIComponent*> PickComponents(UIComponent* c, const XMINT2& pos) noexcept { 
		return g_baseTraverser.PickComponents(c, pos); }
	vector<IMouseEventReceiver*> PickMouseReceivers(UIComponent* c, const XMINT2& pos) noexcept { 
		return g_baseTraverser.PickMouseReceivers(c, pos); }

	////////////////////////////////////////////////////////////////
	// DerivedTraverser

	bool UpdatePositionsManually(UIComponent* c, bool isRoot) noexcept {
		return g_derivedTraverser.UpdatePositionsManually(c, isRoot); }
	bool ChangeSize(UIComponent* c, const XMUINT2& size, bool isForce) noexcept {
		return g_derivedTraverser.ChangeSize(c, size, isForce); }
	unique_ptr<UIComponent> Clone(UIComponent* c) noexcept {
		return g_derivedTraverser.Clone(c); }

	bool ChangeSize(UIComponent* c, uint32_t x, uint32_t y, bool isForce) noexcept { return ChangeSize(c, { x, y }, isForce); }
	bool ChangeSizeX(UIComponent* c, uint32_t v) noexcept { return ChangeSize(c, { v, c->GetSize().y }); }
	bool ChangeSizeX(UIComponent* c, const XMUINT2& s) noexcept { return ChangeSizeX(c, s.x); }
	bool ChangeSizeY(UIComponent* c, uint32_t v) noexcept { return ChangeSize(c, { c->GetSize().x, v }); }
	bool ChangeSizeY(UIComponent* c, const XMUINT2& s) noexcept { return ChangeSizeY(c, s.y); }

	////////////////////////////////////////////////////////////////
	// NameTraverser

	unique_ptr<UIComponent> AttachComponent(UIComponent* parent,
		unique_ptr<UIComponent> child, const XMINT2& relativePos) noexcept {
		return g_nameTraverser.AttachComponent(parent, std::move(child), relativePos); }
	pair<unique_ptr<UIComponent>, UIComponent*> DetachComponent(UIComponent* c) noexcept {
		return g_nameTraverser.DetachComponent(c); }
	UIComponent* FindComponent(UIComponent* c, const string& name) noexcept {
		return g_nameTraverser.FindComponent(c, name); }
	UIComponent* FindRegionComponent(UIComponent* c, const string& region) noexcept {
		return g_nameTraverser.FindRegionComponent(c, region); }
	bool Rename(UIComponent* c, const string& name) noexcept {
		return g_nameTraverser.Rename(c, name); }
	bool RenameRegion(UIComponent* c, const string& region) noexcept {
		return g_nameTraverser.RenameRegion(c, region); }

	unique_ptr<UIComponent> UITraverser::AttachComponent(UIComponent* c, const string& region, 
	const string& name, unique_ptr<UIComponent> child, const XMINT2& relativePos) noexcept
	{
		UIComponent* find = FindComponent(c, region, name);
		if (!find) return child;

		return AttachComponent(find, move(child), relativePos);
	}

	pair<unique_ptr<UIComponent>, UIComponent*> DetachComponent(UIComponent* c,
		const string& region, const string& name) noexcept
	{
		UIComponent* find = FindComponent(c, region, name);
		if (!find) return {};

		return DetachComponent(find);
	}

	UIComponent* FindComponent(UIComponent* c, const string& region, const string& name) noexcept
	{
		UIComponent* component = FindRegionComponent(c, region);
		if (component == nullptr) return nullptr;

		return FindComponent(component, name);
	}
}
