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