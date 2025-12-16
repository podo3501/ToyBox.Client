#pragma once

enum class TextureSlice;
struct SourceDivider;
struct TextureFontInfo;
struct TextureSourceInfo;
class TextureResourceBinder;

optionalRef<vector<Rectangle>> GetSourcesOfBindKey(TextureResourceBinder* rb, const string& key) noexcept;
XMUINT2 GetSizeOfBindKey(TextureResourceBinder* rb, const string& key) noexcept;
SourceDivider GetSourceDivider(TextureResourceBinder* rb, const string& key) noexcept;
SourceDivider GetSourceDivider(const TextureSourceInfo& sourceInfo) noexcept;
pair<wstring, TextureFontInfo> GetTextureFontInfo(TextureResourceBinder* rb, const wstring& filename) noexcept;
vector<pair<string, TextureSourceInfo>> GetTextureSourceInfo(TextureResourceBinder* rb, const wstring& filename) noexcept;
vector<TextureSourceInfo> GetAreas(TextureResourceBinder* rb, const wstring& filename, TextureSlice part) noexcept;
vector<Rectangle> GetAreas(TextureResourceBinder* rb, const wstring& filename, TextureSlice texSlice, const XMINT2& position) noexcept;

vector<Rectangle> ComputeSliceRects(TextureSlice texSlice, const SourceDivider& sourceDivider) noexcept;