#include "pch.h"
#include "HierarchyTraverser.h"
#include "../UIComponent.h"

void HierarchyTraverser::ForEachChild(UIComponent* c, const function<void(UIComponent*)>& Func) noexcept
{
	Func(c);
	for (const auto& child : c->GetChildren())
		ForEachChild(child, Func);
}

void HierarchyTraverser::ForEachChildConst(UIComponent* c, const function<void(const UIComponent*)>& Func) const noexcept
{
	Func(c);
	for (const auto& child : c->GetChildren())
		ForEachChildConst(child, Func);
}

void HierarchyTraverser::ForEachChildBool(UIComponent* c, const function<TraverseResult(UIComponent*)>& Func) noexcept
{
	if (Func(c) == TraverseResult::Stop) return;

	for (auto& child : c->GetChildren())
		ForEachChildBool(child, Func);
}

void HierarchyTraverser::ForEachRenderChildBFS(UIComponent* c, const function<TraverseResult(UIComponent*)>& Func) noexcept
{
	queue<UIComponent*> cQueue;
	auto PushChild = [&cQueue](UIComponent* comp) { if (comp->HasStateFlag(StateFlag::Render)) cQueue.push(comp); };

	PushChild(c);

	while (!cQueue.empty())
	{
		UIComponent* current = cQueue.front();
		cQueue.pop();

		if (current->GetRenderSearchType() == RenderTraversal::DFS)
		{
			ForEachRenderChildDFS(current, Func);
			continue;
		}

		auto result = Func(current);
		Assert(result != TraverseResult::Stop); //Stop 리턴값이 와서는 안된다. 이건 매프레임 렌더링 되는 함수이기 때문에 속도가 중요하다.
		if (result == TraverseResult::ChildrenSkip) continue;

		for (auto child : current->GetChildren())
			PushChild(child);
	}
}

void HierarchyTraverser::ForEachRenderChildDFS(UIComponent* c, const function<TraverseResult(UIComponent*)>& Func) noexcept
{
	auto result = Func(c);
	Assert(result != TraverseResult::Stop); //Stop 리턴값이 와서는 안된다. 이건 매프레임 렌더링 되는 함수이기 때문에 속도가 중요하다.
	if (result == TraverseResult::ChildrenSkip) return;

	for (auto child : c->GetChildren())
		ForEachRenderChildDFS(child, Func);
}

bool HierarchyTraverser::ForEachChildPostUntilFail(UIComponent* c, 
	const function<bool(UIComponent*)>& Func) noexcept
{
	for (auto& child : c->GetChildren())
		if (child && !ForEachChildPostUntilFail(child, Func))
			return false;

	if (!Func(c))
		return false;

	return true;
}

void HierarchyTraverser::ForEachChildToRender(UIComponent* c, const function<TraverseResult(UIComponent*)>& Func) noexcept
{
	RenderTraversal traversal = c->GetRenderSearchType();
	if (traversal == RenderTraversal::BFS || traversal == RenderTraversal::Inherited)
		return ForEachRenderChildBFS(c, Func);

	return ForEachRenderChildDFS(c, Func);
}