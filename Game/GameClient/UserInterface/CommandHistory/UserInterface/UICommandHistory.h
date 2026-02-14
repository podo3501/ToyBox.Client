#pragma once
#include "../Command.h"
#include "../CommandHistory.h"

class UIModule;
class TextureResourceBinder;
class TextArea;
class PatchTexture;
class PatchTextureStd;
class TextureSwitcher;
class UICommandHistory : public CommandHistory<UICommand>
{
public:
	~UICommandHistory();
	UICommandHistory();

	unique_ptr<UIComponent> AttachComponent(UIComponent* parent, 
		unique_ptr<UIComponent> component, const XMINT2& relativePos = {});
	pair<unique_ptr<UIComponent>, UIComponent*> DetachComponent(UIComponent* detach);
	bool ChangeRelativePosition(UIComponent* component, const XMINT2& relativePos);
	bool SetSize(UIComponent* component, const XMUINT2& size);
	bool RenameRegion(UIComponent* component, const string& region);
	bool Rename(UIComponent* component, const string& name);
	bool SetText(TextArea* textArea, const wstring& text);
	bool FitToTextureSource(PatchTexture* patchTex);
	bool FitToTextureSource(TextureSwitcher* texSwitcher);
	bool ChangeBindKey(PatchTextureStd* patchTexStd, TextureResourceBinder* resBinder, const string& key);
	bool ChangeBindKey(TextureSwitcher* texSwitcher, TextureResourceBinder* resBinder, const string& key);
};