#include "pch.h"
#include "SampleComponent.h"
#include "../UIUtility.h"
#include "UserInterface/UIComponent/Traverser/UITraverser.h"
#include "UserInterface/UIComponent/Traverser/UIDetailTraverser.h"
#include "PatchTexture/PatchTextureStd/PatchTextureStd1.h"
#include "PatchTexture/PatchTextureStd/PatchTextureStd3.h"
#include "TextArea.h"
#include "ListArea.h"
#include "ScrollBar.h"
#include "TextureSwitcher.h"
#include "Shared/Utils/StringExt.h"

static map<InteractState, unique_ptr<UIComponent>> GetComponentKeyMap(
	const XMUINT2& size, const string& bindKey,
	function<unique_ptr<UIComponent>(UILayout&, const string&)> CreatePatchTextureFn)
{
	UILayout layout{ size };
	map<InteractState, unique_ptr<UIComponent>> components;

	const auto& stateKeys = GetStateKeyMap(bindKey);
	for (const auto& pair : stateKeys)
	{
		auto component = CreatePatchTextureFn(layout, pair.second);
		if (!component) return {}; // 실패 시 빈 맵 반환

		components.emplace(pair.first, move(component));
	}
	return components;
}

map<InteractState, unique_ptr<UIComponent>> GetComponentKeyMap(const XMUINT2& size, const string& bindKey)
{
	return GetComponentKeyMap(size, bindKey,
		[](UILayout& layout, const string& key) { return CreateComponent<PatchTextureStd1>(layout, key); });
}

map<InteractState, unique_ptr<UIComponent>> GetComponentKeyMap(DirectionType dirType, const XMUINT2& size, const string& bindKey)
{
	return GetComponentKeyMap(size, bindKey,
		[dirType](UILayout& layout, const string& key) { return CreateComponent<PatchTextureStd3>(layout, dirType, key); });
}

unique_ptr<ScrollBar> CreateSampleScrollBar(const UILayout& layout, DirectionType dirType,
	const string& trackKey, const string& buttonKey, TextureSwitcher** outButton)
{
	UILayout gridLayout{ layout.GetSize() };

	auto button = CreateComponent<TextureSwitcher>(gridLayout, DirTypeToTextureSlice(dirType), GetStateKeyMap(buttonKey));
	if (outButton) *outButton = button.get();
		
	return CreateComponent<ScrollBar>(layout, CreateComponent<PatchTextureStd3>(gridLayout, dirType, trackKey), move(button));
}

unique_ptr<ListArea> CreateSampleListArea(const UILayout& layout,
	const string& backImageKey, const string& switcherKey, const string& scrollTrackKey, const string& scrollButtonKey, 
	ScrollBar** outScrollBar)
{
	auto scrollBar = CreateSampleScrollBar({}, DirectionType::Vertical, scrollTrackKey, scrollButtonKey);
	if (outScrollBar) *outScrollBar = scrollBar.get();

	return CreateComponent<ListArea>(layout,
		CreateComponent<PatchTextureStd1>(UILayout{ layout.GetSize() }, backImageKey),
		CreateComponent<TextureSwitcher>(TextureSlice::Nine, GetStateKeyMap(switcherKey)),
		move(scrollBar));
}

bool MakeSampleListAreaData(ITextureController* texController, TextureResourceBinder* rb, ListArea* listArea, int itemCount)
{
	ZoneScopedN("MakeSampleListAreaData");

	//글자가 크기에 안 맞으면 안찍힌다. 
	vector<wstring> bindKeys{ L"Hangle", L"English" };
	auto protoTextArea = CreateComponent<TextArea>(texController, UILayout{ {200, 30}, Origin::LeftTop }, L"", bindKeys);
	ReturnIfFalse(UIDetailTraverser::BindTextureSourceInfo(protoTextArea.get(), rb));
	
	protoTextArea->SetName("ListTextArea");
	auto prototype = listArea->GetPrototypeContainer();
	auto failed = prototype->AttachComponentToCenter(move(protoTextArea), { 0, -6 });
	if (failed) return false; //실패하면 Component가 반환된다. attach는 nullptr이 나와야 잘 붙은 것이다.

	for (auto idx : views::iota(0, itemCount))
	{
		auto container = listArea->PrepareContainer();
		TextArea* textArea = UITraverser::FindComponent<TextArea*>(container, "ListTextArea");
		textArea->SetText(L"<English><Black>Test " + IntToWString(idx * 10) + L"</Black></English>");
	}

	return true;
}