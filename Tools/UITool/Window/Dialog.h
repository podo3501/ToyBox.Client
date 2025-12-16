#pragma once

enum class FileDialogType
{
	Open,  // 파일 열기
	Save   // 파일 저장
};

enum class DialogType
{
	Init,
	Alert,
	Message,
	Error
};

namespace Tool
{
	class Dialog
	{
	public:
		static bool ShowFileDialog(wstring& filename, FileDialogType type);
		static void ShowInfoDialog(const DialogType dialogType, const string& m_msg) noexcept;
		static void Render() noexcept;
		inline static bool IsOpenDialog() noexcept { return m_dialogType != DialogType::Init; }

	private:
		static DialogType m_dialogType;
		static string m_msg;
	};
}

void GetRelativePathFromDialog(wstring& outFilename);