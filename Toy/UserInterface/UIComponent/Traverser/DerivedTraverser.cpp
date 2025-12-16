#include "pch.h"
#include "DerivedTraverser.h"
#include "../UIComponent.h"
#include "Shared/Utils/GeometryExt.h"

DerivedTraverser::~DerivedTraverser() = default;
DerivedTraverser::DerivedTraverser() = default;

unique_ptr<UIComponent> DerivedTraverser::Clone(UIComponent* c)
{
	auto clone = c->Clone();
	UpdatePositionsManually(clone.get());
	return clone;
}
//크기를 바꾸면 이 컴포넌트의 자식들의 위치값도 바꿔준다.
bool DerivedTraverser::ChangeSize(UIComponent* c, const XMUINT2& size, bool isForce) noexcept
{
	XMUINT2 lockedSize{ size };
	const auto& preSize = c->GetSize();
	if (c->HasStateFlag(StateFlag::X_SizeLocked)) lockedSize.x = preSize.x;
	if (c->HasStateFlag(StateFlag::Y_SizeLocked)) lockedSize.y = preSize.y;

	if (!isForce && lockedSize == preSize) return true;

	ReturnIfFalse(c->ResizeAndAdjustPos(size));
	return c->ChangeSize(lockedSize, isForce);
}

bool DerivedTraverser::UpdatePositionsManually(UIComponent* c, bool isRoot) noexcept
{
	UIComponent* component = (isRoot) ? c->GetRoot() : c;
	return RecursivePositionUpdate(component);
}

void DerivedTraverser::PropagateRoot(UIComponent* c, UIComponent* root) noexcept
{
	UIComponent* realRoot = root->m_root;
	ForEachChild(c, [realRoot](UIComponent* component) {
		component->m_root = realRoot;
		});
}

bool DerivedTraverser::BindTextureSourceInfo(UIComponent* c, TextureResourceBinder* resBinder) noexcept
{
	auto forEachResult = ForEachChildPostUntilFail(c, [resBinder](UIComponent* component) {
		bool result = component->BindSourceInfo(resBinder);
		AssertMsg(result, "Failed to load texture");
		return result;
		});
	ReturnIfFalse(forEachResult);
	ReturnIfFalse(UpdatePositionsManually(c));
	return true;
}

bool DerivedTraverser::Update(UIComponent* c, const DX::StepTimer& timer) noexcept
{
	return RecursiveUpdate(c, timer);
}

void DerivedTraverser::Render(UIComponent* c, ITextureRender* render)
{
	//9방향 이미지는 같은 레벨인데 9방향 이미지 위에 다른 이미지를 올렸을 경우 BFS가 아니면 밑에 이미지가 올라온다.
	//가장 밑에 레벨이 가장 위에 올라오는데 DFS(Depth First Search)이면 가장 밑에 있는게 가장 나중에 그려지지 않게 된다.
	ForEachChildToRender(c, [render](UIComponent* component) {
		component->Render(render);

		return (component->GetTypeID() != ComponentID::RenderTexture)
			? TraverseResult::Continue
			: TraverseResult::ChildrenSkip; //RenderTexture이면 자식들은 랜더하지 않는다. RenderTexture에 랜더링 됐기 때문에.
		});
}

bool DerivedTraverser::EnableToolMode(UIComponent* c, bool enable) noexcept
{
	auto modeFunc = enable ? &UIComponent::EnterToolMode : &UIComponent::ExitToolMode;
	return ForEachChildPostUntilFail(c, [enable, modeFunc](UIComponent* component) {
		if (component->m_toolMode == enable) return true;

		component->m_toolMode = enable;
		ReturnIfFalse((component->*modeFunc)());
		return true;
		});
}

bool DerivedTraverser::RecursivePositionUpdate(UIComponent* c, const XMINT2& position) noexcept
{
	const auto& startPos = c->GetTransform().GetUpdatedPosition(c->m_layout, position);
	return ranges::all_of(c->GetChildren(), [this, &startPos](auto& child) {
		auto childStartPos = startPos + child->GetTransform().GetRelativePosition();
		return RecursivePositionUpdate(child, childStartPos);
		});
}

bool DerivedTraverser::RecursiveUpdate(UIComponent* c, const DX::StepTimer& timer, const XMINT2& position) noexcept
{
	if (!c->HasStateFlag(StateFlag::Update)) return true;

	const auto& startPos = c->GetTransform().GetUpdatedPosition(c->m_layout, position);
	ReturnIfFalse(c->Update(timer));

	return ranges::all_of(c->GetChildren(), [this, &timer, &startPos](auto& child) {
		auto childStartPos = startPos + child->GetTransform().GetRelativePosition();
		return RecursiveUpdate(child, timer, childStartPos);
		});
}