#include "pch.h"
#include "StringExt.h"

wstring StringToWString(const string& str) noexcept
{
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
	return wstrTo;
}

static string RemoveNullTerminator(const string& str) noexcept
{
	size_t end = str.find('\0');
	return (end == std::string::npos) ? str : str.substr(0, end);
}

string WStringToString(const wstring& wstr) noexcept
{
	int bufferSize = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
	std::string str(bufferSize, 0);
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], bufferSize, nullptr, nullptr);
	string curStr = string(str);
	return RemoveNullTerminator(curStr);
}

void StringToChar(const string& str, span<char> outstr) noexcept
{
	strncpy_s(outstr.data(), outstr.size() - 1, str.c_str(), _TRUNCATE);
	outstr[outstr.size() - 1] = '\0';
}

void WStringToChar(const wstring& wstr, span<char> outstr) noexcept
{
	StringToChar(WStringToString(wstr), outstr);
}

wstring CharToWString(const span<char> str) noexcept
{
	return StringToWString(string(str.data(), str.size()));
}

wstring IntToWString(int value) noexcept
{
	return StringToWString(to_string(value));
}

string AppendIfPresent(const string& base, const string& suffix, const string& delim)
{
    return suffix.empty() ? base : base + delim + suffix;
}

static pair<string_view, string_view> SplitNameAndId(string_view name)
{
    auto underscore = name.find('_');
    if (underscore == string_view::npos) return { name, {} };

    auto prefix = name.substr(0, underscore);
    auto idStr = name.substr(underscore + 1);
    if (idStr.empty() || !std::ranges::all_of(idStr, ::isdigit)) return {};

    return { prefix, idStr };
}

pair<string, int> ExtractNameAndId(string_view name)
{
    auto [prefix, idStr] = SplitNameAndId(name);
    if (prefix.empty()) return { "", 0 };

    int id = (idStr.empty()) ? 0 : stoi(string(idStr));
    return { string(prefix), id };
}