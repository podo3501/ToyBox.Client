#include "pch.h"
#include "UICommandHistory.h"
#include "UICommandRegistry.h"
#include "Toy/UserInterface/UIComponent/UIComponent.h"
#include "Toy/UserInterface/UIComponent/UIType.h"

UICommandHistory::~UICommandHistory() = default;
UICommandHistory::UICommandHistory() = default;

unique_ptr<UIComponent> UICommandHistory::AttachComponent(	UIComponent* parent,
	unique_ptr<UIComponent> component, const XMINT2& relativePos)
{
	unique_ptr<UICommand> command = make_unique<AttachComponentCommand>(parent, move(component), relativePos);
	if (!command->Execute())
	{
		auto attachCmd = static_cast<AttachComponentCommand*>(command.get());
		return attachCmd->GetFailureResult();
	}

	AddOrMergeCommand(move(command));
	return nullptr;
}

pair<unique_ptr<UIComponent>, UIComponent*> UICommandHistory::DetachComponent(UIComponent* detach)
{
	unique_ptr<UICommand> command = make_unique<DetachComponentCommand>(detach);
	command->Execute();

	auto detachCmd = static_cast<DetachComponentCommand*>(command.get());
	auto [detached, parent] = detachCmd->GetResult();
	if (detached)
		AddOrMergeCommand(move(command));

	return { move(detached), parent };
}

bool UICommandHistory::ChangeRelativePosition(UIComponent* component, const XMINT2& relativePos)
{ return ApplyCommand<ChangeRelativePositionCommand>(component, relativePos); }

bool UICommandHistory::SetSize(UIComponent* component, const XMUINT2& size)
{ return ApplyCommand<SetSizeCommand>(component, size); }

bool UICommandHistory::RenameRegion(UIComponent* component, const string& region)
{ return ApplyCommand<RenameRegionCommand>(component, region); }

bool UICommandHistory::Rename(UIComponent* component, const string& name)
{ return ApplyCommand<RenameCommand>(component, name); }

bool UICommandHistory::SetText(TextArea* textArea, const wstring& text)
{ return ApplyCommand<SetTextCommand>(textArea, text); }

bool UICommandHistory::FitToTextureSource(PatchTexture* patchTex)
{ return ApplyCommand<FitToTextureSourceCommand>(patchTex); }

bool UICommandHistory::FitToTextureSource(TextureSwitcher* texSwitcher)
{ return ApplyCommand<FitToTextureSourceCommand>(texSwitcher); }

bool UICommandHistory::ChangeBindKey(PatchTextureStd* patchTexStd, TextureResourceBinder* resBinder, const string& key)
{ return ApplyCommand<ChangeBindKeyCommand>(patchTexStd, resBinder, key); }

bool UICommandHistory::ChangeBindKey(TextureSwitcher* texSwitcher, TextureResourceBinder* resBinder, const string& key)
{ return ApplyCommand<ChangeBindKeyCommand>(texSwitcher, resBinder, key); }