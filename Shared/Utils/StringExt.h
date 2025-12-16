#pragma once

wstring StringToWString(const string& str) noexcept;
//string RemoveNullTerminator(const string& str) noexcept;
string WStringToString(const wstring& wstr) noexcept;
void StringToChar(const string& str, span<char> outstr) noexcept;
wstring CharToWString(const span<char> str) noexcept;
void WStringToChar(const wstring& wstr, span<char> outstr) noexcept;
wstring IntToWString(int value) noexcept;

string AppendIfPresent(const string& base, const string& suffix, const string& delim = "_");
pair<string, int> ExtractNameAndId(string_view name);
