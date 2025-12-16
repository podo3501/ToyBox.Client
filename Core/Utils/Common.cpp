#include "pch.h"
#include "Common.h"

CException::CException(HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber) :
    ErrorCode(hr),
    FunctionName(functionName),
    Filename(filename),
    LineNumber(lineNumber)
{}

std::wstring CException::ToString() const
{
    // Get the string description of the error code.
    _com_error err(ErrorCode);
    std::wstring msg = err.ErrorMessage();

    return FunctionName + L" failed in " + Filename + L"; line " + std::to_wstring(LineNumber) + L"; error: " + msg;
}

////////////////////////////////////////////////////////////////
//CycleIterator

CycleIterator::CycleIterator(int s, int e) : start(s), end(e), current(s)
{
    assert(s < e);
}

static int mod(int value, int range)
{
    return (value % range + range) % range;
}

int CycleIterator::GetCurrent() const noexcept
{
    return current;
}

int CycleIterator::Increase() noexcept
{
    current = start + mod(current + 1 - start, end - start);
    return current;
}

int CycleIterator::Decrease() noexcept
{
    current = start + mod(current - 1 - start, end - start);
    return current;
}

void MergeRectangles(vector<Rectangle>& rects) noexcept
{
    if (rects.empty()) return;

    bool merged;
    do
    {
        merged = false;
        vector<Rectangle> tempResult;

        while (!rects.empty())
        {
            Rectangle current = rects.back();
            rects.pop_back();

            // 기존에 합쳐진 직사각형을 찾는 함수
            auto it = ranges::find_if(tempResult, [&current](const Rectangle& res) {
                return res.Intersects(current); 
                });

            if (it != tempResult.end()) // 합쳐질 직사각형을 찾은 경우
            {
                *it = Rectangle::Union(*it, current); // 합친 직사각형 업데이트
                merged = true;
            }
            else
                tempResult.push_back(current); // 합쳐지지 않은 직사각형은 추가
        }

        rects = std::move(tempResult);
    } while (merged);
}