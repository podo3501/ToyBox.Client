#include "pch.h"
#include "StringUtils.h"

void ToLower(std::string& s)
{
    std::transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
}

void ToUpper(std::string& s)
{
    std::transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
}

std::string ToLowerCopy(std::string s)
{
    ToLower(s);
    return s;
}

std::string ToUpperCopy(std::string s)
{
    ToUpper(s);
    return s;
}

std::string ToLower(std::string_view s)
{
    std::string result(s);
    ToLower(result);
    return result;
}

std::string ToUpper(std::string_view s)
{
    std::string result(s);
    ToUpper(result);
    return result;
}