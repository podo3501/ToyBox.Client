#pragma once

class ToolLoop;
class RecentFiles;
class ResolutionSetting;
class InnerWindow;
enum class FileMenuAction;

class FileTab
{
public:
	FileTab(ToolLoop* ToolLoop);
	~FileTab();

	void Show();
	bool Excute();
	bool CreateUIWindowFromFile(const wstring& filename);

private:
	InnerWindow* GetFocusWindow() const noexcept;
	void HandleFileMenuAction(FileMenuAction action);
	bool CreateNewUIFile() noexcept;
	bool CreateUIWindow();
	bool CreateTextureResBinderWindow(const wstring& filename = L"");
	bool CreateTextureWindow();
	bool Save(InnerWindow* focusWnd, const wstring& filename = L"") const;
	bool SaveFocusWindow();
	bool SaveAsFocusWindow();
	bool SetResolution();

	ToolLoop* m_toolLoop;
	unique_ptr<RecentFiles> m_recentFiles;
	optional<FileMenuAction> m_currentAction; // 현재 메뉴 상태를 저장
};
