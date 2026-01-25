#pragma once
#include "../Command.h"
#include "../CommandType.h"
#include "../../TextureResourceBinder/TextureFontInfo.h"
#include "../../TextureResourceBinder/TextureSourceInfo.h"

class TextureResourceBinder;

struct TextureFontInfo;
class AddFontKeyCommand : public TexResCommand
{
public:
	AddFontKeyCommand(TextureResourceBinder* rb, const wstring& key, const TextureFontInfo& info) noexcept;

	virtual bool Execute() override;
	virtual bool Undo() override;
	virtual bool Redo() override;

protected:
	virtual TexResCommandID GetTypeID() const noexcept override { return TexResCommandID::AddFontKey; }
	virtual bool IsMerge(TexResCommand*) noexcept { return false; }

private:
	wstring m_key;
	CommandRecord<TextureFontInfo> m_record;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RemoveFontKeyCommand : public TexResCommand
{
public:
	RemoveFontKeyCommand(TextureResourceBinder* rb, const wstring& key) noexcept;

	virtual bool Execute() override;
	virtual bool Undo() override;
	virtual bool Redo() override;

protected:
	virtual TexResCommandID GetTypeID() const noexcept override { return TexResCommandID::RemoveFontKey; }
	virtual bool IsMerge(TexResCommand*) noexcept { return false; }

private:
	wstring m_key;
	CommandRecord<TextureFontInfo> m_record;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class AddTextureKeyCommand : public TexResCommand
{
public:
	AddTextureKeyCommand(TextureResourceBinder* rb, const string& key, const TextureSourceInfo& info) noexcept;

	virtual bool Execute() override;
	virtual bool Undo() override;
	virtual bool Redo() override;

protected:
	virtual TexResCommandID GetTypeID() const noexcept override { return TexResCommandID::AddTextureKey; }
	virtual bool IsMerge(TexResCommand*) noexcept { return false; }

private:
	string m_key;
	CommandRecord<TextureSourceInfo> m_record;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RemoveTextureKeyCommand : public TexResCommand
{
public:
	RemoveTextureKeyCommand(TextureResourceBinder* rb, const string& key) noexcept;

	virtual bool Execute() override;
	virtual bool Undo() override;
	virtual bool Redo() override;

protected:
	virtual TexResCommandID GetTypeID() const noexcept override { return TexResCommandID::RemoveTextureKey; }
	virtual bool IsMerge(TexResCommand*) noexcept { return false; }

private:
	string m_key;
	CommandRecord<TextureSourceInfo> m_record;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RemoveKeyByFilenameCommand : public TexResCommand
{
public:
	RemoveKeyByFilenameCommand(TextureResourceBinder* rb, const wstring& filename) noexcept;

	virtual bool Execute() override;
	virtual bool Undo() override;
	virtual bool Redo() override;

protected:
	virtual TexResCommandID GetTypeID() const noexcept override { return TexResCommandID::RemoveKeyByFilename; }
	virtual bool IsMerge(TexResCommand*) noexcept { return false; }

private:
	using FontInfo = pair<wstring, TextureFontInfo>;
	using TextureInfos = vector<pair<string, TextureSourceInfo>>;

	wstring m_filename;
	CommandRecord<pair<FontInfo, TextureInfos>> m_record;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RenameFontKeyCommand : public TexResCommand
{
public:
	RenameFontKeyCommand(TextureResourceBinder* rb, const wstring& preKey, const wstring& newKey) noexcept;

	virtual bool Execute() override;
	virtual bool Undo() override;
	virtual bool Redo() override;

protected:
	virtual TexResCommandID GetTypeID() const noexcept override { return TexResCommandID::RenameFontKey; }
	virtual bool IsMerge(TexResCommand*) noexcept { return false; }

private:
	CommandRecord<pair<wstring, wstring>> m_record;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RenameTextureKeyCommand : public TexResCommand
{
public:
	RenameTextureKeyCommand(TextureResourceBinder* rb, const string& preKey, const string& newKey) noexcept;

	virtual bool Execute() override;
	virtual bool Undo() override;
	virtual bool Redo() override;

protected:
	virtual TexResCommandID GetTypeID() const noexcept override { return TexResCommandID::RenameTextureKey; }
	virtual bool IsMerge(TexResCommand*) noexcept { return false; }

private:
	CommandRecord<pair<string, string>> m_record;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ModifyTexSrcInfoCommand : public TexResCommand
{
public:
	ModifyTexSrcInfoCommand(TextureResourceBinder* rb, const string& key, const TextureSourceInfo& sourceInfo) noexcept;

	virtual bool Execute() override;
	virtual bool Undo() override;
	virtual bool Redo() override;

protected:
	virtual TexResCommandID GetTypeID() const noexcept override { return TexResCommandID::ModifyTexSrcInfo; }
	virtual void PostMerge(unique_ptr<TexResCommand> other) noexcept override;

private:
	string m_key;
	CommandRecord<TextureSourceInfo> m_record;
};


