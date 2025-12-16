#include "pch.h"
#include "TexResCommandHistory.h"
#include "TexResCommandRegistry.h"

TexResCommandHistory::~TexResCommandHistory() = default;
TexResCommandHistory::TexResCommandHistory(TextureResourceBinder* resBinder) noexcept :
	m_resBinder{ resBinder }
{}

bool TexResCommandHistory::AddFontKey(const wstring& key, const TextureFontInfo& info)
{ return ApplyCommand<AddFontKeyCommand>(m_resBinder, key, info); }

bool TexResCommandHistory::RemoveFontKey(const wstring& key)
{ return ApplyCommand<RemoveFontKeyCommand>(m_resBinder, key); }

bool TexResCommandHistory::AddTextureKey(const string& key, const TextureSourceInfo& info)
{ return ApplyCommand<AddTextureKeyCommand>(m_resBinder, key, info); }

bool TexResCommandHistory::RemoveTextureKey(const string& key)
{ return ApplyCommand<RemoveTextureKeyCommand>(m_resBinder, key); }

bool TexResCommandHistory::RemoveKeyByFilename(const wstring& filename)
{ return ApplyCommand<RemoveKeyByFilenameCommand>(m_resBinder, filename); }

bool TexResCommandHistory::RenameFontKey(const wstring& preKey, const wstring& newKey)
{ return ApplyCommand<RenameFontKeyCommand>(m_resBinder, preKey, newKey); }

bool TexResCommandHistory::RenameTextureKey(const string& preKey, const string& newKey)
{ return ApplyCommand<RenameTextureKeyCommand>(m_resBinder, preKey, newKey); }

bool TexResCommandHistory::ModifyTextureSourceInfo(const string& key, const TextureSourceInfo& info)
{ return ApplyCommand<ModifyTexSrcInfoCommand>(m_resBinder, key, info); }