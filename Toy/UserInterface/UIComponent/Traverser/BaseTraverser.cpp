#include "pch.h"
#include "BaseTraverser.h"
#include "../UIComponent.h"
#include "Shared/Utils/GeometryExt.h"

XMUINT2 BaseTraverser::GetChildrenBoundsSize(UIComponent* c) noexcept
{
	Rectangle totalArea{ c->GetArea() }; //초기값을 지정하지 않으면 0, 0 부터 시작하는 큰 사각형이 union된다.
	ForEachChildConst(c, [&totalArea](const UIComponent* child) {
		const auto& area = child->GetArea();
		totalArea = Rectangle::Union(totalArea, area);
		});

	return GetSizeFromRectangle(totalArea);
}

vector<UIComponent*> BaseTraverser::PickComponents(UIComponent* c, const XMINT2& pos) noexcept
{
	vector<UIComponent*> findList;
	ForEachChildToRender(c, [&findList, &pos](UIComponent* comp) {
		const bool inside = Contains(comp->GetArea(), pos);

		if (comp->GetTypeID() == ComponentID::RenderTexture && !inside)
			return TraverseResult::ChildrenSkip; // RenderTexture는 영역 밖이면 자식 탐색 중단

		if (inside) // 영역 안이면 리스트에 추가
			findList.push_back(comp);

		return TraverseResult::Continue;
		});
	ranges::reverse(findList); //앞으로 넣어주는 것보다 push_back 하고 reverse 하는게 더 빠르다. vector가 단순 배열이라 캐쉬가 좋기 때문에 이걸로 한다.	 
	return findList;
}

vector<IMouseEventReceiver*> BaseTraverser::PickMouseReceivers(UIComponent* c, const XMINT2& pos) noexcept
{
	vector<IMouseEventReceiver*> findList;
	ForEachChildToRender(c, [&findList, &pos](UIComponent* comp) {
		if (comp->GetTypeID() == ComponentID::RenderTexture && !Contains(comp->GetArea(), pos))
			return TraverseResult::ChildrenSkip; // RenderTexture 영역 밖이면 자식 탐색 중단

		if (auto eventReceiver = comp->AsMouseEventReceiver(); eventReceiver) { // 마우스 이벤트 수신 가능한지 먼저 확인
			if (Contains(comp->GetArea(), pos))
				findList.push_back(eventReceiver);
		}

		return TraverseResult::Continue;
		});
	ranges::reverse(findList); //앞으로 넣어주는 것보다 push_back 하고 reverse 하는게 더 빠르다. vector가 단순 배열이라 캐쉬가 좋기 때문에 이걸로 한다.	 
	return findList;
}