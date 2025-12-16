#pragma once
#include "../Command.h"
#include "../CommandType.h"
#include "../../UIComponent/UIType.h"

class UIModule;
class UIComponent;
class AttachComponentCommand : public UICommand
{
public:
	AttachComponentCommand(UIComponent* parent, unique_ptr<UIComponent> component, const XMINT2& relativePos) noexcept;

	virtual bool Execute() override;
	virtual bool Undo() override;
	virtual bool Redo() override;

	unique_ptr<UIComponent> GetFailureResult() noexcept;

protected:
	virtual UICommandID GetTypeID() const noexcept override { return UICommandID::AttachComponent; }
	virtual bool IsMerge(UICommand*) noexcept { return false; }

private:
	UIComponent* m_parent;
	unique_ptr<UIComponent> m_attach;
	XMINT2 m_pos{};
	UIComponent* m_detach;
	unique_ptr<UIComponent> m_failureResult;
};

class DetachComponentCommand : public UICommand
{
public:
	DetachComponentCommand(UIComponent* detach) noexcept;

	virtual bool Execute() override;
	virtual bool Undo() override;
	virtual bool Redo() override;

	pair<unique_ptr<UIComponent>, UIComponent*> GetResult() noexcept;

protected:
	virtual UICommandID GetTypeID() const noexcept override { return UICommandID::DetachComponent; }
	virtual bool IsMerge(UICommand*) noexcept { return false; }

private:
	UIComponent* m_detach;
	XMINT2 m_position{};
	unique_ptr<UIComponent> m_component;
	UIComponent* m_parent;
	pair<unique_ptr<UIComponent>, UIComponent*> m_result;
};

class ChangeRelativePositionCommand : public UICommand
{
public:
	ChangeRelativePositionCommand(UIComponent* component, const XMINT2& relativePos) noexcept;

	virtual bool Execute() override;
	virtual bool Undo() override;
	virtual bool Redo() override;

protected:
	virtual UICommandID GetTypeID() const noexcept override { return UICommandID::ChangeRelativePosition; }
	virtual void PostMerge(unique_ptr<UICommand> other) noexcept override;

private:
	CommandRecord<XMINT2> m_record;
};

class SetSizeCommand : public UICommand
{
public:
	SetSizeCommand(UIComponent* component, const XMUINT2& size) noexcept;

	virtual bool Execute() override;
	virtual bool Undo() override;
	virtual bool Redo() override;

protected:
	virtual UICommandID GetTypeID() const noexcept override { return UICommandID::SetSize; }
	virtual void PostMerge(unique_ptr<UICommand> other) noexcept override;

private:
	CommandRecord<XMUINT2> m_record;
};

class RenameRegionCommand : public UICommand
{
public:
	RenameRegionCommand(UIComponent* component, const string& region) noexcept;

	virtual bool Execute() override;
	virtual bool Undo() override;
	virtual bool Redo() override;

protected:
	virtual UICommandID GetTypeID() const noexcept override { return UICommandID::SetRegion; }
	virtual bool IsMerge(UICommand*) noexcept { return false; }

private:
	CommandRecord<string> m_record;
};

class RenameCommand : public UICommand
{
public:
	RenameCommand(UIComponent* component, const string& name) noexcept;

	virtual bool Execute() override;
	virtual bool Undo() override;
	virtual bool Redo() override;

protected:
	virtual UICommandID GetTypeID() const noexcept override { return UICommandID::Rename; }
	virtual bool IsMerge(UICommand*) noexcept { return false; }

private:
	CommandRecord<string> m_record;
};

class TextArea;
class SetTextCommand : public UICommand
{
public:
	SetTextCommand(TextArea* textArea, const wstring& text) noexcept;

	virtual bool Execute() override;
	virtual bool Undo() override;
	virtual bool Redo() override;

protected:
	virtual UICommandID GetTypeID() const noexcept override { return UICommandID::SetText; }
	virtual bool IsMerge(UICommand*) noexcept { return false; }

private:
	TextArea* m_textArea; //UIComponent* 가 아닌 이상은 template 상속일때 문제가 되어서 변수를 두었다. 나중에 해결할 수 있는지 봐도 좋을 것이다.
	CommandRecord<wstring> m_record;
};

class PatchTexture;
class TextureSwitcher;
class FitToTextureSourceCommand : public UICommand
{
public:
	FitToTextureSourceCommand(PatchTexture* patchTex) noexcept;
	FitToTextureSourceCommand(TextureSwitcher* texSwitcher) noexcept;

	virtual bool Execute() override;
	virtual bool Undo() override;
	virtual bool Redo() override;

protected:
	virtual UICommandID GetTypeID() const noexcept override { return m_commandID; }
	virtual bool IsMerge(UICommand*) noexcept { return false; }

private:
	template<typename FuncT>
	bool WithTarget(FuncT&& Func);

	UICommandID m_commandID{ UICommandID::Unknown };
	XMUINT2 m_size{};
};

class PatchTextureStd;
class TextureResourceBinder;
class ChangeBindKeyCommand : public UICommand
{
public:
	ChangeBindKeyCommand(PatchTextureStd* patchTexStd, TextureResourceBinder* resBinder, const string& key) noexcept;
	ChangeBindKeyCommand(TextureSwitcher* texSwitcher, TextureResourceBinder* resBinder, const string& key) noexcept;

	virtual bool Execute() override;
	virtual bool Undo() override;
	virtual bool Redo() override;

protected:
	virtual UICommandID GetTypeID() const noexcept override { return UICommandID::Unknown; }
	virtual bool IsMerge(UICommand*) noexcept { return false; }

private:
	template<typename FuncT>
	bool WithTarget(FuncT&& Func);

	UICommandID m_commandID{ UICommandID::Unknown };
	TextureResourceBinder* m_resBinder;
	XMUINT2 m_prevSize{};
	CommandRecord<string> m_record;
};