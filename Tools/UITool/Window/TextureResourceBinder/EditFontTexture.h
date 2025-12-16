#pragma once

enum class PendingAction;
class TexResCommandHistory;
class EditListBox;
class RenameNotifier;
class EditFontTexture
{
public:
	~EditFontTexture();
	EditFontTexture();

	void Update() noexcept;
	void Render();
	void SetCommandHistory(TexResCommandHistory* cmdHistory) noexcept { m_cmdHistory = cmdHistory; }

private:
	inline bool IsVaildFontIndex() const noexcept { return m_fontIndex >= 0 && m_fontIndex < m_fontFiles.size(); }
	wstring GetSelectFontFile() const noexcept;

	TexResCommandHistory* m_cmdHistory;
	vector<wstring> m_fontFiles;
	int m_fontIndex{ -1 };
	unique_ptr<EditListBox> m_listboxFont;
	unique_ptr<RenameNotifier> m_renameNotifier;
};