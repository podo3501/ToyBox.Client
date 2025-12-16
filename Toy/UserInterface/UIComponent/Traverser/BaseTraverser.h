#pragma once
#include "HierarchyTraverser.h"

struct IMouseEventReceiver;
class UIComponent;
class BaseTraverser : private HierarchyTraverser //Derived도 Name도 아닌 일반적인
{
public:
	XMUINT2 GetChildrenBoundsSize(UIComponent* c) noexcept;
	vector<UIComponent*> PickComponents(UIComponent* c, const XMINT2& pos) noexcept;
	vector<IMouseEventReceiver*> PickMouseReceivers(UIComponent* c, const XMINT2& pos) noexcept;

private:
};

