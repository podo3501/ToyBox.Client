#pragma once
#include "HierarchyTraverser.h"

class UIComponent;
class MouseEventReceiver;
class BaseTraverser : private HierarchyTraverser //Derived도 Name도 아닌 일반적인
{
public:
	XMUINT2 GetChildrenBoundsSize(UIComponent* c) noexcept;
	vector<UIComponent*> PickComponents(UIComponent* c, const XMINT2& pos) noexcept;
	vector<MouseEventReceiver*> PickMouseReceivers(UIComponent* c, const XMINT2& pos) noexcept;

private:
};

