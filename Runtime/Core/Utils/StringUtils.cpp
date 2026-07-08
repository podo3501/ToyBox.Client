#include "pch.h"
#include "StringUtils.h"

void Core::ToLower(std::string& s)
{
    std::transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
}

void Core::ToUpper(std::string& s)
{
    std::transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
}

std::string Core::ToLowerCopy(std::string s)
{
    ToLower(s);
    return s;
}

std::string Core::ToUpperCopy(std::string s)
{
    ToUpper(s);
    return s;
}

std::string Core::ToLower(std::string_view s)
{
    std::string result(s);
    ToLower(result);
    return result;
}

std::string Core::ToUpper(std::string_view s)
{
    std::string result(s);
    ToUpper(result);
    return result;
}

std::string_view Core::GetExtension(std::string_view path)
{
    size_t slashPos = path.find_last_of("/\\");
    size_t dotPos = path.find_last_of('.');

    if (dotPos == std::string_view::npos)
        return {};

    if (slashPos != std::string_view::npos &&
        dotPos < slashPos)
    {
        return {};
    }

    return path.substr(dotPos);
}

std::wstring Core::UTF8ToWString(const std::string& str)
{
    if (str.empty())
        return L"";

    const char8_t* u8Str = reinterpret_cast<const char8_t*>(str.data());
    size_t length = str.size();

    std::wstring result;
    result.reserve(length);

    size_t i = 0;
    while (i < length)
    {
        char8_t c = u8Str[i];

        // ASCII 영역 (1바이트 문자: 0xxxxxxx)
        if ((c & 0x80) == 0)
        {
            result.push_back(static_cast<wchar_t>(c));
            i += 1;
        }
        // 2바이트 문자 (110xxxxx 10xxxxxx)
        else if ((c & 0xE0) == 0xC0)
        {
            if (i + 1 < length)
            {
                wchar_t uni = (u8Str[i] & 0x1F) << 6;
                uni |= (u8Str[i + 1] & 0x3F);
                result.push_back(uni);
            }
            i += 2;
        }
        // 3바이트 문자 (한글 영역 핵심: 1110xxxx 10xxxxxx 10xxxxxx)
        else if ((c & 0xF0) == 0xE0)
        {
            if (i + 2 < length)
            {
                wchar_t uni = (u8Str[i] & 0x0F) << 12;
                uni |= (u8Str[i + 1] & 0x3F) << 6;
                uni |= (u8Str[i + 2] & 0x3F);
                result.push_back(uni);
            }
            i += 3;
        }
        // 4바이트 문자 (이모지 및 고대 문자: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx)
        else if ((c & 0xF8) == 0xF0)
        {
            if (i + 3 < length)
            {
                // UTF-16 대리 쌍(Surrogate Pair) 처리
                uint32_t utf32 = (u8Str[i] & 0x07) << 18;
                utf32 |= (u8Str[i + 1] & 0x3F) << 12;
                utf32 |= (u8Str[i + 2] & 0x3F) << 6;
                utf32 |= (u8Str[i + 3] & 0x3F);

                utf32 -= 0x10000;
                result.push_back(static_cast<wchar_t>((utf32 >> 10) + 0xD800));
                result.push_back(static_cast<wchar_t>((utf32 & 0x3FF) + 0xDC00));
            }
            i += 4;
        }
        else
        {
            // 잘못된 잘못된 바이트스트림 예외 처리
            i += 1;
        }
    }

    return result;
}

std::vector<char32_t> Core::UTF8ToUTF32(std::string_view text)
{
    std::vector<char32_t> result;
    result.reserve(text.size());

    const auto* bytes =
        reinterpret_cast<const unsigned char*>(text.data());

    size_t i = 0;

    while (i < text.size())
    {
        char32_t codePoint = 0;
        unsigned char c = bytes[i];

        if (c <= 0x7F) // 1 byte (ASCII)
        {
            codePoint = c;
            i += 1;
        }
        else if ((c & 0xE0) == 0xC0) // 2 bytes
        {
            if (i + 1 >= text.size())
                break;

            codePoint =
                ((c & 0x1F) << 6) |
                (bytes[i + 1] & 0x3F);

            i += 2;
        }
        else if ((c & 0xF0) == 0xE0) // 3 bytes (한글 대부분)
        {
            if (i + 2 >= text.size())
                break;

            codePoint =
                ((c & 0x0F) << 12) |
                ((bytes[i + 1] & 0x3F) << 6) |
                (bytes[i + 2] & 0x3F);

            i += 3;
        }
        else if ((c & 0xF8) == 0xF0) // 4 bytes
        {
            if (i + 3 >= text.size())
                break;

            codePoint =
                ((c & 0x07) << 18) |
                ((bytes[i + 1] & 0x3F) << 12) |
                ((bytes[i + 2] & 0x3F) << 6) |
                (bytes[i + 3] & 0x3F);

            i += 4;
        }
        else
        {
            ++i;
            continue; // 잘못된 UTF-8
        }

        result.push_back(codePoint);
    }

    return result;
}

std::string Core::UTF32ToUTF8(std::span<const char32_t> text)
{
    std::string result;
    result.reserve(text.size() * 3);

    for (char32_t c : text)
    {
        uint32_t codePoint = static_cast<uint32_t>(c);

        if (codePoint <= 0x7F)
        {
            result.push_back(static_cast<char>(codePoint));
        }
        else if (codePoint <= 0x7FF)
        {
            result.push_back(static_cast<char>(0xC0 | (codePoint >> 6)));
            result.push_back(static_cast<char>(0x80 | (codePoint & 0x3F)));
        }
        else if (codePoint <= 0xFFFF)
        {
            result.push_back(static_cast<char>(0xE0 | (codePoint >> 12)));
            result.push_back(static_cast<char>(0x80 | ((codePoint >> 6) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | (codePoint & 0x3F)));
        }
        else if (codePoint <= 0x10FFFF)
        {
            result.push_back(static_cast<char>(0xF0 | (codePoint >> 18)));
            result.push_back(static_cast<char>(0x80 | ((codePoint >> 12) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | ((codePoint >> 6) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | (codePoint & 0x3F)));
        }
        else
        {
            result.push_back('?');
        }
    }
    result.shrink_to_fit(); 

    return result;
}