#pragma once
#include "../Command.h"
#include "../CommandHistory.h"

struct TextureFontInfo;
struct TextureSourceInfo;
class TextureResourceBinder;
class TexResCommandHistory : public CommandHistory<TexResCommand>
{
public:
	~TexResCommandHistory();
	TexResCommandHistory() = delete;
	explicit TexResCommandHistory(TextureResourceBinder* resBinder) noexcept;

	bool AddFontKey(const wstring& bindingKey, const TextureFontInfo& fontInfo);
	bool RemoveFontKey(const wstring& key);

	bool AddTextureKey(const string& key, const TextureSourceInfo& info);
	bool RemoveTextureKey(const string& key);

	bool RemoveKeyByFilename(const wstring& filename);

	bool RenameFontKey(const wstring& preKey, const wstring& newKey);
	bool RenameTextureKey(const string& preKey, const string& newKey);

	bool ModifyTextureSourceInfo(const string& key, const TextureSourceInfo& info);
	inline TextureResourceBinder* GetReceiver() const noexcept { return m_resBinder; }

private:
	TextureResourceBinder* m_resBinder;
};
