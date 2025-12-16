#include "pch.h"
#include "NameTraverser.h"
#include "DerivedTraverser.h"
#include "UserInterface/UIModule.h"
#include "UserInterface/UIComponent/Components/Panel.h"
#include "UserInterface/UINameGenerator/UINameGenerator.h"

NameTraverser::NameTraverser() = default;
unique_ptr<UIComponent> NameTraverser::AttachComponent(UIComponent* parent,
	unique_ptr<UIComponent> child, const XMINT2& relativePos) noexcept
{
	if (!parent->HasStateFlag(StateFlag::Attach))
		return move(child);

	if (UINameGenerator* nameGen = GetNameGenerator(parent))
	{
		bool success = ForEachChildWithRegion(child.get(), GetMyRegion(parent),
			[nameGen](const string& region, UIComponent* component, bool isNewRegion) {
				const string& name = component->GetName().empty() ? EnumToString<ComponentID>(component->GetTypeID()) : component->GetName();
				auto namesOpt = nameGen->MakeNameOf(region, name, isNewRegion);
				if (!namesOpt) return false;
				const auto& [newRegion, newName] = *namesOpt;

				if (isNewRegion) component->SetRegion(newRegion);
				component->SetName(newName);
				return true;
			});

		if (!success)
			return move(child);
	}

	DerivedTraverser derivedTraverser;
	derivedTraverser.PropagateRoot(child.get(), parent);
	auto resChild = parent->AttachComponent(move(child), relativePos);
	if (resChild != nullptr) //attach가 되지 못했다면
		return resChild;

	derivedTraverser.UpdatePositionsManually(parent, true);
	return nullptr;
}

pair<unique_ptr<UIComponent>, UIComponent*> NameTraverser::DetachComponent(UIComponent* c) noexcept
{
	UIComponent* parent = c->GetParent();
	if (!parent || !parent->HasStateFlag(StateFlag::Detach)) return {};

	if (UINameGenerator* nameGen = GetNameGenerator(parent))
	{
		bool allRemoved = ForEachChildWithRegion(c, GetMyRegion(parent),
			[nameGen](const string& region, UIComponent* component, bool isNewRegion) {
				if (nameGen->IsUnusedRegion(region)) //region이 없는 경우는 detach 하다가 부모 노드에서 region을 지운 경우이다.
					return true;

				if (isNewRegion) //노드가 새로운 region이면 
					return nameGen->RemoveRegion(region);
				return nameGen->RemoveName(region, component->GetName());
			});

		if (!allRemoved)
			return {};
	}
	
	auto [resDetached, resParent] = c->DetachComponent();
	if (!resDetached) return {};

	DerivedTraverser derivedTraverser;
	derivedTraverser.PropagateRoot(resDetached.get(), resDetached.get());
	derivedTraverser.UpdatePositionsManually(resDetached.get());
	return { move(resDetached), resParent };
}

UIComponent* NameTraverser::FindComponent(UIComponent* c, const string& name) noexcept
{
	UIComponent* root = GetRegionRoot(c);
	const string& region = root->GetRegion();
	UIComponent* find = nullptr;

	ForEachChildBool(root, [this, &find, &name, &region](UIComponent* component) {
		const string& curRegion = component->GetRegion();
		if (!curRegion.empty() && region != curRegion) return TraverseResult::Stop; //Region 루트가 아닌 새로운 region이 나왔을때 

		if (component->GetName() == name)
		{
			find = component;
			return TraverseResult::Stop;
		}

		return TraverseResult::Continue;
		});

	return find;
}

UIComponent* NameTraverser::FindRegionComponent(UIComponent* c, const string& region) noexcept
{
	UIComponent* find{ nullptr };

	ForEachChildBool(c->GetRoot(), [&find, &region](UIComponent* c) {
		if (c->GetRegion() == region)
		{
			find = c;
			return TraverseResult::Stop;
		}
		return TraverseResult::Continue;
		});

	return find;
}

bool NameTraverser::Rename(UIComponent* c, const string& name) noexcept
{
	UINameGenerator* nameGen = GetNameGenerator(c);
	if (!nameGen) return false;

	const auto& region = GetMyRegion(c);
	if (!nameGen->IsUnusedName(region, name)) return false;

	ReturnIfFalse(nameGen->RemoveName(region, c->GetName()));
	auto namesOpt = nameGen->MakeNameOf(region, name);
	if (!namesOpt) return false;
	
	c->SetName(namesOpt->second);
	return true;
}

bool NameTraverser::RenameRegion(UIComponent* c, const string& newRegion) noexcept
{
	UINameGenerator* nameGen = GetNameGenerator(c);
	if (!nameGen) return false;

	const string oldRegion = c->GetRegion();
	if (oldRegion == newRegion)
		return true;

	UIComponent* parentRoot = GetParentRegionRoot(c);
	if (newRegion == "") // region이 "" 라면 기존 region을 삭제 처리한다.
		return RemoveAndMergeRegion(nameGen, c, parentRoot, oldRegion);

	//새 region 이름이 이미 존재하면 중복 처리
	if (!nameGen->IsUnusedRegion(newRegion))
		return false;

	return ReplaceAndMergeRegion(nameGen, c, parentRoot, oldRegion, newRegion);
}

void NameTraverser::AssignNamesInRegion(UINameGenerator* nameGen, 
	UIComponent* component, const string& region) noexcept
{
	ForEachChildInSameRegion(component, [nameGen, &region, component](UIComponent* curComponent) {
		auto namesOpt = nameGen->MakeNameOf(region, curComponent->GetName());
		curComponent->SetName(namesOpt->second);
		if (curComponent == component)
			curComponent->SetRegion(namesOpt->first);
		});
}

bool NameTraverser::RemoveAndMergeRegion(UINameGenerator* nameGen, UIComponent* c,
	UIComponent* parentRoot, const string& oldRegion) noexcept
{
	// 기존 region 제거
	ReturnIfFalse(nameGen->RemoveRegion(oldRegion));
	c->SetRegion(""); //region을 삭제한다.

	// root 여부에 따라 재정렬 대상 결정
	UIComponent* target = (c == parentRoot) ? c : parentRoot;
	const string& targetRegion = target->GetRegion();

	ReturnIfFalse(nameGen->RemoveRegion(targetRegion));
	AssignNamesInRegion(nameGen, target, targetRegion);
	return true;
}

bool NameTraverser::ReplaceAndMergeRegion(UINameGenerator* nameGen, UIComponent* c,
	UIComponent* parentRoot, const string& oldRegion, const string& newRegion) noexcept
{
	//기존 region을 제거하고 새 region 재할당
	ReturnIfFalse(nameGen->RemoveRegion(oldRegion));
	AssignNamesInRegion(nameGen, c, newRegion);

	//이전에 region이 없었던 경우 상위도 갱신
	if (oldRegion == "" && c != parentRoot)
	{
		string parentRegion = parentRoot->GetRegion();
		ReturnIfFalse(nameGen->RemoveRegion(parentRegion));
		AssignNamesInRegion(nameGen, parentRoot, parentRegion);
	}

	return true;
}

UINameGenerator* NameTraverser::GetNameGenerator(UIComponent* c) noexcept
{
	Panel* panel = ComponentCast<Panel*>(c->GetRoot());
	if (!panel) return nullptr;

	auto uiModule = panel->GetUIModule();
	return uiModule ? uiModule->GetNameGenerator() : nullptr;
}

string NameTraverser::GetMyRegion(UIComponent* c) const noexcept
{
	UIComponent* regionRoot = GetRegionRoot(c);
	return regionRoot->GetRegion();
}

bool NameTraverser::ForEachChildWithRegion(UIComponent* c, const string& parentRegion, const function<bool(const string&, UIComponent*, bool)>& Func) noexcept
{
	const auto Traverse = [&](UIComponent* component, const string& inheritedRegion, auto&& self_ref) -> bool {
		string currentRegion = component->GetRegion().empty() ? inheritedRegion : component->GetRegion();
		bool isNewRegion{ currentRegion != inheritedRegion };
		if (!isNewRegion && !component->GetRegion().empty()) // region이 같더라도, component가 region을 "명시적으로" 지정했다면 새 region으로 간주
			isNewRegion = true;
		ReturnIfFalse(Func(currentRegion, component, isNewRegion)); //새로운 region을 생성하려면 isNewRegion은 true

		const string& updatedRegion = component->GetRegion();
		const string nextRegion = updatedRegion.empty() ? currentRegion : updatedRegion;

		for (auto& child : component->GetChildren())
			ReturnIfFalse(self_ref(child, nextRegion, self_ref));
		return true;
		};

	string curRegion = !parentRegion.empty() ? parentRegion : GetMyRegion(c);
	return Traverse(c, curRegion, Traverse);
}

void NameTraverser::ForEachChildInSameRegion(UIComponent* c, const function<void(UIComponent*)>& Func) noexcept
{
	const auto Traverse = [&](UIComponent* component, const string& region, auto&& self_ref) -> void {
		const string& nodeRegion = component->GetRegion();
		if (!nodeRegion.empty() && nodeRegion != region)
			return;

		Func(component);

		for (auto& child : component->GetChildren())
			self_ref(child, region, self_ref);
		};

	UIComponent* regionComponent = GetRegionRoot(c);
	Traverse(c, regionComponent->GetRegion(), Traverse);
}

UIComponent* NameTraverser::GetRegionRoot(UIComponent* c) const noexcept
{
	while (c->GetRegion().empty()) //값이 있으면 RegionRoot 이다.
	{
		auto parent = c->GetParent();
		if (!parent) break; //끝까지 올라가서 없으면 root 를 리턴
		c = parent;
	}

	return c;
}

UIComponent* NameTraverser::GetParentRegionRoot(UIComponent* c) const noexcept
{
	auto parent = c->GetParent();
	if (!parent) return c;

	return GetRegionRoot(parent);
}