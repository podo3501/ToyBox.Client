#pragma once

struct ITextureController;
class UIComponent;
class UILayout;
class ListArea;
class ScrollBar;
class TextureSwitcher;
class TextureResourceBinder;
enum class InteractState;
enum class DirectionType;

map<InteractState, unique_ptr<UIComponent>> GetComponentKeyMap(const XMUINT2& size, const string& bindKey);
map<InteractState, unique_ptr<UIComponent>> GetComponentKeyMap(DirectionType dirType, const XMUINT2& size, const string& bindKey);

unique_ptr<ScrollBar> CreateSampleScrollBar(const UILayout& layout, DirectionType dirType,
	const string& trackKey = "ScrollTrack3_V", const string& buttonKey = "ScrollButton3_V", TextureSwitcher** outButton = nullptr);
unique_ptr<ListArea> CreateSampleListArea(const UILayout& layout,
	const string& backImageKey = "ListBackImage1", const string& switcherKey = "ListBackground9",
	const string& scrollTrackKey = "ScrollTrack3_V", const string& scrollButtonKey = "ScrollButton3_V",
	ScrollBar** outScrollBar = nullptr);

bool MakeSampleListAreaData(ITextureController* texController, TextureResourceBinder* rb, ListArea* listArea, int itemCount);