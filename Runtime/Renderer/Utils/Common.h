#pragma once

class CException
{
public:
    CException() = default;
    CException(HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber);

    std::wstring ToString()const;

    HRESULT ErrorCode = S_OK;
    std::wstring FunctionName;
    std::wstring Filename;
    int LineNumber = -1;
};

template <typename E>
constexpr auto
Ev(E enumerator) noexcept
{
    return static_cast<std::underlying_type_t<E>>(enumerator);
}

void MergeRectangles(vector<Rectangle>& rects) noexcept;