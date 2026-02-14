#pragma once
#include "HierarchyTraverser.h"

class UIComponent;
class UINameGenerator;
class NameTraverser : private HierarchyTraverser //이름관련. Traverser은 멤벼변수를 가질 수 없다.
{
public:
	NameTraverser();
	unique_ptr<UIComponent> AttachComponent(UIComponent* parent,
		unique_ptr<UIComponent> child, const XMINT2& relativePos) noexcept;
	pair<unique_ptr<UIComponent>, UIComponent*> DetachComponent(UIComponent* c) noexcept;
	UIComponent* FindComponent(UIComponent* c, const string& name) noexcept;
	UIComponent* FindRegionComponent(UIComponent* c, const string& region) noexcept;
	bool Rename(UIComponent* c, const string& name) noexcept;
	bool RenameRegion(UIComponent* component, const string& region) noexcept;

private:
	UINameGenerator* GetNameGenerator(UIComponent* c) noexcept;
	void AssignNamesInRegion(UINameGenerator* nameGen, UIComponent* component, const string& region) noexcept;
	bool RemoveAndMergeRegion(UINameGenerator* nameGen, UIComponent* c, 
		UIComponent* parentRoot, const string& oldRegion) noexcept;
	bool ReplaceAndMergeRegion(UINameGenerator* nameGen, UIComponent* c, 
		UIComponent* parentRoot, const string& oldRegion, const string& newRegion) noexcept;
	string GetMyRegion(UIComponent* c) const noexcept;
	UIComponent* GetRegionRoot(UIComponent* c) const noexcept;
	UIComponent* GetParentRegionRoot(UIComponent* c) const noexcept;

	bool ForEachChildWithRegion(UIComponent* c, const string& parentRegion, const function<bool(const string&, UIComponent*, bool)>& Func) noexcept;
	void ForEachChildInSameRegion(UIComponent* c, const function<void(UIComponent*)>& Func) noexcept;
};
