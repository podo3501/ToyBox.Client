#include "pch.h"
#include "TexResCommandRegistry.h"
#include "IRenderer.h"
#include "../../TextureResourceBinder/TextureResourceBinder.h"

AddFontKeyCommand::AddFontKeyCommand(TextureResourceBinder* rb, const wstring& key, const TextureFontInfo& info) noexcept :
	TexResCommand{ rb }, m_key{ key }, m_record{ info }
{}
bool AddFontKeyCommand::Execute()
{
	m_record.previous = {};
	GetTarget()->AddFontKey(m_key, m_record.current);
	return true;
}
bool AddFontKeyCommand::Undo() { GetTarget()->RemoveFontKey(m_key); return true; }
bool AddFontKeyCommand::Redo() { return GetTarget()->AddFontKey(m_key, m_record.current); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RemoveFontKeyCommand::RemoveFontKeyCommand(TextureResourceBinder* rb, const wstring& key) noexcept :
	TexResCommand{ rb }, m_key{ key }
{
}
bool RemoveFontKeyCommand::Execute()
{
	auto infoRef = GetTarget()->GetTextureFontInfo(m_key);
	ReturnIfFalse(infoRef);

	m_record.previous = infoRef->get();
	GetTarget()->RemoveFontKey(m_key);
	return true;
}
bool RemoveFontKeyCommand::Undo() { return GetTarget()->AddFontKey(m_key, m_record.previous); }
bool RemoveFontKeyCommand::Redo() { GetTarget()->RemoveFontKey(m_key); return true; }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

AddTextureKeyCommand::AddTextureKeyCommand(TextureResourceBinder* rb, const string& key, const TextureSourceInfo& info) noexcept :
	TexResCommand{ rb }, m_key{ key }, m_record{ info }
{}
bool AddTextureKeyCommand::Execute()
{
	m_record.previous = {};
	GetTarget()->AddTextureKey(m_key, m_record.current);
	return true;
}
bool AddTextureKeyCommand::Undo() { GetTarget()->RemoveTextureKey(m_key); return true; }
bool AddTextureKeyCommand::Redo() { return GetTarget()->AddTextureKey(m_key, m_record.current); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RemoveTextureKeyCommand::RemoveTextureKeyCommand(TextureResourceBinder* rb, const string& key) noexcept :
	TexResCommand{ rb }, m_key{ key }
{}
bool RemoveTextureKeyCommand::Execute()
{
	auto infoRef = GetTarget()->GetTextureSourceInfo(m_key);
	ReturnIfFalse(infoRef);

	m_record.previous = infoRef->get();
	GetTarget()->RemoveTextureKey(m_key);
	return true;
}
bool RemoveTextureKeyCommand::Undo() { return GetTarget()->AddTextureKey(m_key, m_record.previous); }
bool RemoveTextureKeyCommand::Redo() { GetTarget()->RemoveTextureKey(m_key); return true; }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RemoveKeyByFilenameCommand::RemoveKeyByFilenameCommand(TextureResourceBinder* rb, const wstring& filename) noexcept :
	TexResCommand{ rb }, m_filename{ filename }
{}

bool RemoveKeyByFilenameCommand::Execute()
{
	pair<wstring, TextureFontInfo> fontInfo = GetTextureFontInfo(GetTarget(), m_filename);
	vector<pair<string, TextureSourceInfo>> sourceInfos = GetTextureSourceInfo(GetTarget(), m_filename);

	m_record.previous = pair{ fontInfo, sourceInfos };
	return GetTarget()->RemoveKeyByFilename(m_filename);
}

bool RemoveKeyByFilenameCommand::Undo()
{ 
	const pair<FontInfo, TextureInfos>& infoPair = m_record.previous;
	const auto& fontInfo = infoPair.first;
	GetTarget()->AddFontKey(fontInfo.first, fontInfo.second);

	const auto& texInfos = infoPair.second;
	for (const auto& texInfo : texInfos)
		GetTarget()->AddTextureKey(texInfo.first, texInfo.second);
	
	return true;
}
bool RemoveKeyByFilenameCommand::Redo() { return GetTarget()->RemoveKeyByFilename(m_filename); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RenameFontKeyCommand::RenameFontKeyCommand(TextureResourceBinder* rb, const wstring& preKey, const wstring& newKey) noexcept :
	TexResCommand{ rb }, m_record{ pair(preKey, newKey) }
{}
bool RenameFontKeyCommand::Execute()
{
	m_record.previous = { m_record.current.second, m_record.current.first };
	return GetTarget()->RenameFontKey(m_record.current.first, m_record.current.second);
}
bool RenameFontKeyCommand::Undo() { return GetTarget()->RenameFontKey(m_record.previous.first, m_record.previous.second); }
bool RenameFontKeyCommand::Redo() { return GetTarget()->RenameFontKey(m_record.current.first, m_record.current.second); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RenameTextureKeyCommand::RenameTextureKeyCommand(TextureResourceBinder* rb, const string& preKey, const string& newKey) noexcept :
	TexResCommand{ rb }, m_record{ pair(preKey, newKey) }
{}
bool RenameTextureKeyCommand::Execute()
{
	m_record.previous = { m_record.current.second, m_record.current.first };
	return GetTarget()->RenameTextureKey(m_record.current.first, m_record.current.second);
}
bool RenameTextureKeyCommand::Undo() { return GetTarget()->RenameTextureKey(m_record.previous.first, m_record.previous.second); }
bool RenameTextureKeyCommand::Redo() { return GetTarget()->RenameTextureKey(m_record.current.first, m_record.current.second); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ModifyTexSrcInfoCommand::ModifyTexSrcInfoCommand(TextureResourceBinder* rb, const string& key, const TextureSourceInfo& info) noexcept :
	TexResCommand{ rb }, m_key{ key }, m_record { info }
{}
bool ModifyTexSrcInfoCommand::Execute()
{
	auto infoRef = GetTarget()->GetTextureSourceInfo(m_key);
	ReturnIfFalse(infoRef);

	m_record.previous = infoRef->get();
	return GetTarget()->ModifyTextureSourceInfo(m_key, m_record.current);
}
bool ModifyTexSrcInfoCommand::Undo() { return GetTarget()->ModifyTextureSourceInfo(m_key, m_record.previous); }
bool ModifyTexSrcInfoCommand::Redo() { return GetTarget()->ModifyTextureSourceInfo(m_key, m_record.current); }

void ModifyTexSrcInfoCommand::PostMerge(unique_ptr<TexResCommand> other) noexcept
{
	auto otherCmd = static_cast<ModifyTexSrcInfoCommand*>(other.get());
	m_record.current = otherCmd->m_record.current;
}