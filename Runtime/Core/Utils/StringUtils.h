#pragma once

namespace Core
{
	void ToLower(std::string& s);
	void ToUpper(std::string& s);

	std::string ToLowerCopy(std::string s);
	std::string ToUpperCopy(std::string s);

	std::string ToLower(std::string_view s);
	std::string ToUpper(std::string_view s);

	std::string_view GetExtension(std::string_view path);
	std::wstring UTF8ToWString(const std::string& str);

	char32_t UTF8ToUTF32Char(std::string_view text, size_t& offset);
	std::vector<char32_t> UTF8ToUTF32(std::string_view text);

	void UTF32ToUTF8Char(char32_t c, std::string& out);
	std::string UTF32ToUTF8(std::span<const char32_t> text);
}

