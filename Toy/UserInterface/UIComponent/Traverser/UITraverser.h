#pragma once

struct IMouseEventReceiver;
class UIComponent;
class BaseTraverser;
class DerivedTraverser;
class NameTraverser;
namespace UITraverser
{
	extern BaseTraverser g_baseTraverser;
	extern DerivedTraverser g_derivedTraverser;
	extern NameTraverser g_nameTraverser;

	////////////////////////////////////////////////////////////////
	// BaseTraverser

	XMUINT2 GetChildrenBoundsSize(UIComponent* c) noexcept;
	vector<UIComponent*> PickComponents(UIComponent* c, const XMINT2& pos) noexcept;
	vector<IMouseEventReceiver*> PickMouseReceivers(UIComponent* c, const XMINT2& pos) noexcept;

	////////////////////////////////////////////////////////////////
	// DerivedTraverser

	bool UpdatePositionsManually(UIComponent* c, bool isRoot = false) noexcept;
	bool ChangeSize(UIComponent* c, const XMUINT2& size, bool isForce = false) noexcept;
	unique_ptr<UIComponent> Clone(UIComponent* c) noexcept;

	bool ChangeSize(UIComponent* c, uint32_t x, uint32_t y, bool isForce = false) noexcept;
	bool ChangeSizeX(UIComponent* c, uint32_t v) noexcept;
	bool ChangeSizeX(UIComponent* c, const XMUINT2& s) noexcept;
	bool ChangeSizeY(UIComponent* c, uint32_t v) noexcept;
	bool ChangeSizeY(UIComponent* c, const XMUINT2& s) noexcept;

	////////////////////////////////////////////////////////////////
	// NameTraverser

	unique_ptr<UIComponent> AttachComponent(UIComponent* parent,
		unique_ptr<UIComponent> child, const XMINT2& relativePos = {}) noexcept;
	pair<unique_ptr<UIComponent>, UIComponent*> DetachComponent(UIComponent* c) noexcept;
	UIComponent* FindComponent(UIComponent* c, const string& name) noexcept;
	UIComponent* FindRegionComponent(UIComponent* c, const string& region) noexcept;
	bool Rename(UIComponent* c, const string& name) noexcept;
	bool RenameRegion(UIComponent* c, const string& region) noexcept;

	unique_ptr<UIComponent> AttachComponent(UIComponent* c, const string& region, const string& name,
		unique_ptr<UIComponent> child, const XMINT2& relativePos = {}) noexcept;
	pair<unique_ptr<UIComponent>, UIComponent*> DetachComponent(UIComponent* c,
		const string& region, const string& name) noexcept;
	UIComponent* FindComponent(UIComponent* c, const string& region, const string& name) noexcept;
	template<typename T>
	T FindComponent(UIComponent* c, const string& name) noexcept
	{
		UIComponent* find = FindComponent(c, name);
		return ComponentCast<T>(find);
	}
}