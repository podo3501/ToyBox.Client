#pragma once

enum class UICommandID : int
{
	AttachComponent,
	DetachComponent,
	ChangeRelativePosition,
	SetSize,
	SetRegion,
	Rename,
	SetSource,
	SetSource39,
	SetFilename,
	SetSourceAndDivider,
	SetText,
	FitToTextureSource,
	FitToTextureSourceTS,
	ChangeBindKey,
	ChangeBindKeyTS,
	Unknown
};
class UIComponent;
using UICommand = Command<UICommandID, UIComponent>;

enum class TexResCommandID : int
{
	AddFontKey,
	RemoveFontKey,
	AddTextureKey,
	RemoveTextureKey,
	RemoveKeyByFilename,
	RenameFontKey,
	RenameTextureKey,
	ModifyTexSrcInfo,
	Unknown
};
class TextureResourceBinder;
using TexResCommand = Command<TexResCommandID, TextureResourceBinder>;

template <typename T>
class CommandRecord
{
public:
	CommandRecord() noexcept = default;
	CommandRecord(const T& curr) noexcept : current{ curr } {}
	CommandRecord(T&& curr) noexcept : current(move(curr)) {}

	T current{};
	T previous{};
};